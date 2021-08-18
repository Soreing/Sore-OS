#include <kernel/asm.h>
#include <drivers/FAT32.h>
#include <drivers/filesys.h>
#include <drivers/file.h>
#include <drivers/iobuffers.h>
#include <string.h>

#define OFFSET_ATTRIB 0x0B
#define OFFSET_CLUS_HI 0x14
#define OFFSET_CLUS_LO 0x1A
#define OFFSET_SIZE 0x1C
#define FLAG_LFN 0x0F

const char LFN_idxs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};

struct FAT32_HEADER fileSys;

int  FATSecIdx=-1;
int  FATBuffer[128];

int  ClusterSecIdx=-1;
char ClusterBuffer[2048];

char sysPath[256];
  
struct File currentDir;
int dirClusterIndex=-1;

// Identifies if a drive exists and selects it
// Uses ATA PIO mode with command ECh
bool identify()
{
	char status=-1;
	char buffer[512];

	outb(0x1F6, 0xA0);	// Set Master Drive (0xA0) or Slave Drive (0xB0)
	outb(0x1F2, 0x00);	// Set Sector count to 0
	outb(0x1F3, 0x00);	// Set LBA lo to 0
	outb(0x1F4, 0x00);	// Set LBA mid to 0
	outb(0x1F5, 0x00);	// Set LBA hi to 0

	outb(0x1F7, 0xEC);	// Issue IDENTIFY command
	status=inb(0x1F7);	// Wait and Read status

	// If Status is 0, the drive does not exist
	if(status == 0)
	{	return false;
	}

	// Keep polling untill BSY is cleared
	while ((status & 0x80) != 0)
	{	status = inb(0x1F7);
	}

	// If LBAmid and LBAhi are non zero, not an ATA drive
	if (inb(0x1F4)!=0 || inb(0x1F5)!=0)
	{	return false;
	}

	// Keep polling untill DRQ or ERR are set
	while ((status & 0x09) == 0);
	{	status = inb(0x1F7);
	}
	
	// If ERR is clear, read 512 bytes (256 words)
	if((status & 0x01) == 0)
	{	for(long i=0; i<512; i+=2)
		{	*(short*)(buffer+i) = inw(0x1F0);
		}
		return true;
	}
	return false;
}

// Reads sectors starting from some LBA address into a buffer
// Uses ATA PIO mode with command 20h
bool readSector(short sectors, long LBA, char* buffer)
{
	char status=inb(0x1F7);

	// Shall be set to one
	outb(0x1F6, 0x40);

	// Number of sectors to read
	outb(0x1F2, (sectors & 0xFF));

	// LBA position of the data
	outb(0x1F3, (LBA & 0xFF));
	outb(0x1F4, (LBA >> 8 ) & 0xFF);
	outb(0x1F5, (LBA >> 16 ) & 0xFF);

	// READ SECTOR(S) - 20h
	outb(0x1F7, 0x20);
	status = inb(0x1F7);

	// Poll the status while it's BSY, or till DRQ or ERR is set
	while ((status & 0x80) != 0 || (status & 0x09) == 0)
	{	status = inb(0x1F7);
	}

	// If ERR is not set, read the data
	if((status & 0x01) == 0)
	{	for(long i=0; i<512*sectors; i+=2)
		{	*(short*)(buffer+i) = inw(0x1F0);
		}
		return true;
	}
	return false;
}

// Loads a new sector from the File Allocation Table into the memory
// Updates the index of which sector is loaded
bool load_FATSect(int offset)
{
    int LBA_Sector = fileSys.LBA_FAT_Location + offset;
    if(readSector(1, LBA_Sector, (char*)FATBuffer))
    {   FATSecIdx = offset;
        return true;
    }

    return false;
}

// Loads a new Cluster into the memory
// Updates the index of which cluster is loaded
bool load_Cluster(int index)
{
    int clusterSize = fileSys.A_BF_BPB_SectorsPerCluster;
    int offset      = (index-2) * clusterSize;
    int LBA_Sector  = fileSys.LBA_Cluster_Location + offset;

    if(readSector(clusterSize, LBA_Sector, (char*)ClusterBuffer))
    {   ClusterSecIdx = index;
        return true;
    }

    return false;
}

// Traverses the File Allocation Table to find the clusters 
// of a file/directory. Max size of a file is 8 clusters
bool traverseFAT(int startCluster, int clusters[8])
{
    int FAT_offset;
    int cluster_offset;
    clusters[0] = startCluster;

    for(int i=1; i<8; i++)
    {
        if(clusters[i-1] == 0x0FFFFFFF)
        {   break;
        }

        // Load another sector from the FAT if it's out of reach
        // Each Sector contains 128 cluster indexes
        FAT_offset = clusters[i-1] >> 7;
        if(FAT_offset != FATSecIdx)
        {   if(!load_FATSect(FAT_offset))
            {   return false;
            }
        }

        // Set the value of the next cluster using the offset
        // In this sector of the FAT
        cluster_offset = clusters[i-1] & 0x7F;
        clusters[i] = FATBuffer[cluster_offset];
    }

    return true;
}

// Replace the current directory's clusters with a new one
// Reads the first cluster and sets the directory cluster index to 0
bool openDirectory(int startCluster)
{
    if(traverseFAT(startCluster, currentDir.clusters))
    {   if(load_Cluster(startCluster))
        {   dirClusterIndex = 0;
            return true;
        }
    }

    return false;
}

// Loads the file system on a drive and opens the Root directory
// by traversing its cluster indexes and loading the first cluster
bool loadFileSystem()
{
    char buffer[512];

    if(identify())
    {   
        if(readSector(1, 1, buffer))
        {   
            readHeader(&fileSys, buffer);
            if(openDirectory(fileSys.A_BF_BPB_RootDirStrtClus))
            {   sysPath[0]= '/';
                return true;
            }
        }
    }

    return false;
}

// Finds a file in the current directory by name
// Returns the start cluster index of the file or -1 if not found
int findFile(const char* filename, char *type)
{
    int clusterIndex    = 0;
    int recordOffset    = 0;
    int LongFileNameIdx = 0;
    
    char longName[105];
    char shortName[12];
    
    memset(longName, 0, 105);
    memset(shortName, 0, 12);

    int cluster;

    // Load first cluster to start traversal
    if(dirClusterIndex != 0)
    {   if(!load_Cluster(currentDir.clusters[0]))
        {   return -1;
        }
    }

    while(true)
    {   // If there are no more records, break out of the loop
        if(ClusterBuffer[recordOffset] == 0)
        {   break;
        }

        // Logic for interpreting long file names
        if((ClusterBuffer[recordOffset + OFFSET_ATTRIB] & FLAG_LFN) == FLAG_LFN)
        {   // If it's the start of the LFN (4Xh), set LFN index
            if((ClusterBuffer[recordOffset] & 0xF0) == 0x40)
            {   LongFileNameIdx = ClusterBuffer[recordOffset] & 0x0F;
            }

            // Decrement the LFN Index if it matches the Index
            // Exit with error if the LFN index is wrong
            if((ClusterBuffer[recordOffset] & 0x0F) == LongFileNameIdx)
            {   LongFileNameIdx--;
            }
            else
            {   return -1;
            }

            int  bytePos;
            char LFNChar;
            //Read 13 characters into the entry name
            for(int i=0; i<13; i++)
            {   bytePos = recordOffset + LFN_idxs[i];
                LFNChar = ClusterBuffer[bytePos+1] == 0x00 ? ClusterBuffer[bytePos] : 0;
                longName[LongFileNameIdx*13+i] = LFNChar;
            }
        }
        // If it's not a deleted record
        else if(ClusterBuffer[recordOffset] != 0xE5)
        {
            // Copy the short name of the record to the buffer
            memcpy(shortName, ClusterBuffer+recordOffset, 11);

            // Check if the short name or the long name if valid, matches the search term
            // If either name matches, find the cluster number, set the type and return
            if(strcmp(shortName, filename) == 0 || (longName[0] != 0 && strcmp(longName, filename) == 0))
            {   cluster  = (*(unsigned short*)(ClusterBuffer+(recordOffset + OFFSET_CLUS_HI))) << 16;
                cluster |= (*(unsigned short*)(ClusterBuffer+(recordOffset + OFFSET_CLUS_LO)));
                
                *type = ClusterBuffer[recordOffset + OFFSET_ATTRIB] & 0x10 ? FTYPE_DIRECTORY : FTYPE_FILE;
                return cluster;
            }

            memset(longName, 0, 105);
        }

        // Move to the next record
        recordOffset += 32;

        // If the offset reached the end of the cluster, fetch the next one
        // If there are no more clusters, or the load fails, break out of the loop
        if(recordOffset == fileSys.A_BF_BPB_SectorsPerCluster * fileSys.A_BF_BPB_BytesPerSector)
        {   if(dirClusterIndex != 8 && currentDir.clusters[dirClusterIndex+1] != 0x0FFFFFFF)
            {   if(load_Cluster(currentDir.clusters[dirClusterIndex+1]))
                {   dirClusterIndex++;
                    recordOffset += 0;
                }
                // Failed to load cluster
                else
                {   break;
                }
            }
            // End of clusters
            else
            {   break;
            }
        }

    }

    return -1;
}

// Returns the system path
char* getWorkingDirectory()
{   return sysPath;
}