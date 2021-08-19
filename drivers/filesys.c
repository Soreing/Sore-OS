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
unsigned int  FATBuffer[128];

int  ClusterSecIdx=-1;
unsigned char ClusterBuffer[2048];

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

    // For S number of sectors
    for(int s = 0; s < sectors; s++)
    {   
        // Poll the status while it's BSY, or till DRQ or ERR is set
        status = inb(0x1F7);
        while ((status & 0x80) != 0 || (status & 0x09) == 0)
        {	status = inb(0x1F7);
        }

        // If ERR is not set, read the data
        if((status & 0x01) == 0)
        {   for(long i = 0; i < 512; i += 2)
            {*(short*)(buffer+i+s*512) = inw(0x1F0);
            }
        }
        else
        {   return false;
        }
    }

    return true;
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
            {   return true;
            }
        }
    }

    return false;
}

// If an applicable filename is given, creates a ShortFileName
// The SFN is null terminated and needs 12 bytes of space
bool getShortName(const char* filename, char* buffer)
{
    int len = strlen(filename);
    
    int  dotIdx   = 0;
    int  dotCount = 0;
    bool caseOk   = true;

    for(int i=0; i<len; i++)
    {
        // Track the position and count of the dot
        if(filename[i] == '.')
        {   dotCount++;
            dotIdx = i;
        }
        // Check if all the cases are uppercase
        else if(filename[i] < 'A' || filename[i] > 'Z')
        {   caseOk = false;
        }
    }

    // Prepare the short file name buffer
    memset(buffer, ' ', 11);
    buffer[11] = 0;

    // Special case for current directory "."
    if(len == 1 && dotCount == 1)
    {   buffer[0]='.';
        return true;
    }
    // Special case for parent directory ".."
    else if(len == 2 && dotCount == 2)
    {   buffer[0]='.';
        buffer[1]='.';
        return true;
    }
    // If the filename is applicable without extension
    else if(caseOk && dotCount == 0 && len <= 8 )
    {   memcpy(buffer, filename, len);
        return true;
    }
    // If the filename is applicable with extension
    else if(caseOk && dotCount == 1 && dotIdx > 0 && dotIdx <= 9 && (len-dotIdx) <=3 )
    {
        memcpy(buffer, filename, dotIdx-1);
        memcpy(buffer, filename + dotIdx + 1, (len-dotIdx));
        return true;
    }

    return false;
}

// Gets a File object from a start cluster
int getFile(const int startCluster, struct File* file)
{
    traverseFAT(startCluster, file->clusters);
}

// Prints the contents of the file on the screen
void printFile(struct File file)
{
    dirClusterIndex = -1;
    for(int i=0; i<8 && file.clusters[i] != 0x0FFFFFFF;i++)
    {   load_Cluster(file.clusters[i]);
        printStr(ClusterBuffer, 2048);
    }
}

// Finds a file in the current directory by name
// Returns the start cluster index of the file or -1 if not found
int findFile(const char* filename, char *type)
{
    int cluster;
    int clusterIndex    = 0;
    int recordOffset    = 0;
    int LongFileNameIdx = 0;
    
    // If the filename is larger than the max, throw error
    if(strlen(filename) > 104)
    {   return -1;
    }

    char name[105];
    char longName[105];
    char shortName[12];
    
    memset(longName, 0, 105);
    memset(shortName, 0, 12);
    
    // Get either the long name or the short name
    if(!getShortName(filename, name))
    {   strcpy(name, filename);
    }

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

        if( ClusterBuffer[recordOffset] != 0xE5)
        {   
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
            // Else if it's a normal file record
            else
            {   // Copy the short name of the record to the buffer
                memcpy(shortName, ClusterBuffer+recordOffset, 11);

                // Check if the short name or the long name if valid, matches the search term
                // If either name matches, find the cluster number, set the type and return
                if(strcmp(shortName, name) == 0 || (longName[0] != 0 && strcmp(longName, name) == 0))
                {   cluster  = (*(unsigned short*)(ClusterBuffer+(recordOffset + OFFSET_CLUS_HI))) << 16;
                    cluster |= (*(unsigned short*)(ClusterBuffer+(recordOffset + OFFSET_CLUS_LO)));
                    
                    *type = ClusterBuffer[recordOffset + OFFSET_ATTRIB] & 0x10 ? FTYPE_DIRECTORY : FTYPE_FILE;
                    return cluster;
                }

                // Reset long name for the next file
                memset(longName, 0, 105);
            }
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

// Lists files in the current directory
void listFiles()
{
    int clusterIndex    = 0;
    int recordOffset    = 0;
    int LongFileNameIdx = 0;

    char longName[105];
    char shortName[12];
    
    memset(longName, 0, 105);
    memset(shortName, 0, 12);

    // Load first cluster to start traversal
    if(dirClusterIndex != 0)
    {   if(!load_Cluster(currentDir.clusters[0]))
        {   return;
        }
    }

    while(true)
    {   // If there are no more records, break out of the loop
        if(ClusterBuffer[recordOffset] == 0)
        {   break;
        }

        // If the record is not deleted
        if(ClusterBuffer[recordOffset] != 0xE5)
        {
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
                {   return;
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
            // Else if it's a normal file record
            else
            {   //Print either the long (if valid) or the short name
                memcpy(shortName, ClusterBuffer+recordOffset, 11);
                printStrZ(longName[0] == 0 ? shortName : longName);
                printChar('\n');

                // Reset long name for the next file
                memset(longName, 0, 105);
            }
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
}

// Returns the system path
char* getWorkingDirectory()
{   return sysPath[0] == 0 ? "/" : sysPath;
}

// Adds a new directory name to the system path
void addPath(char* name)
{
    int len = strlen(sysPath);
    sysPath[len++] = '/';
    strcpy(sysPath+len, name);
}

// Removes the last directory name from the system path
void delPath()
{
    int i;
    for(i=strlen(sysPath); sysPath[i] != '/'; i--)
    {   sysPath[i]=0;
    }

    sysPath[i] = 0;
}

// Changes the current directory and adjusts the system path
void changeDirectory(char* dirName)
{
    int  cluster;
    char type;

    cluster = findFile(dirName, &type);

    // Adjustment for the root directory
    if(cluster == 0)
    {   cluster = fileSys.A_BF_BPB_RootDirStrtClus;
    }

    if(cluster == -1)
    {   printStrZ("No such file or directory\n");
    }
    else if(type != FTYPE_DIRECTORY)
    {   printStrZ("Not a directory\n");
    }
    else
    {   if(openDirectory(cluster))
        {   if(strcmp(dirName, "..") == 0)
            {   delPath();
            }
            else if(strcmp(dirName, ".") != 0)
            {   addPath(dirName);
            }
        }
    }
}