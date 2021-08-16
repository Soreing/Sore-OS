#include "Format.h"
#include "DiskUtil.h"
#include <fstream>
#include <time.h>

#include <iostream>

// Adds a partition to the buffer
// Uses the details from the file system using a FAT32 Header
// Partitiontable is assumed to be cleared and filled with 0s
void addPartition(const FAT32_HEADER &header, unsigned char* partitiontable)
{
	int endSector = header.A_BF_BPB_BigTotalSectors + 2;

	// Start Sector of the partition (CHS)
	partitiontable[0x1BF] = 0x00;
	partitiontable[0x1C0] = 0x00;
	partitiontable[0x1C1] = 0x02;
	
	// File system type (FAT32)
	partitiontable[0x1C2] = 0x0C;	

	// End Sector of the partition (CHS)
	partitiontable[0x1C3] = endSector / 16065;
	partitiontable[0x1C4] = (endSector / 63) % 255;
	partitiontable[0x1C5] = endSector % 63;

	// Logical Block Addressing start sector
	*(DD*)(partitiontable + 0x1C6) = 0x01;

	// Logical Block Addressing end sector
	*(DD*)(partitiontable + 0x1CA) = header.A_BF_BPB_BigTotalSectors;

	// Boot Signature
	partitiontable[0x1FE] = 0x55;
	partitiontable[0x1FF] = 0xAA;
}

// Adds an FSInfo structure to the buffer
void addFSInfo(const FAT32_HEADER &header, unsigned char* buffer)
{
    // First Signature
    buffer[0x00] = 0x52;
    buffer[0x01] = 0x52;
    buffer[0x02] = 0x61;
    buffer[0x03] = 0x41;

    // Signature of FSInfo Sector
    buffer[0x1E4] = 0x72;
    buffer[0x1E5] = 0x72;
    buffer[0x1E6] = 0x41;
    buffer[0x1E7] = 0x61;

    // Number of free clusters
    *(DD*)(buffer + 0x1E8) = (header.A_BF_BPB_BigTotalSectors / header.A_BF_BPB_SectorsPerCluster) - 1;
    
    // Most Recently allocated cluster
    buffer[0x1EC] = 0x02;

    // Boot Signature
    buffer[0x1FE] = 0x55;
    buffer[0x1FF] = 0xAA;
}

// Adds a FAT32 file system to the buffer
// Filesystem is assumed to be cleared and filled with 0s
void addFileSystem(FAT32_HEADER &header, int hiddenSectors, unsigned char* filesystem)
{
	int now = (int)time(NULL);

    // Calculating time for the Volume ID
	DB ss = now % 60;
	DB mm = (now / 60) % 60;
	DB hh = (now / 3600) % 24;
	DW dd = now / 86400;

    // Setting the Volume ID as teh current timestamp
	*(((DB*)&header.A_BF_BPB_VolumeID) + 3) = ss / 10 * 16 + ss % 10;
	*(((DB*)&header.A_BF_BPB_VolumeID) + 2) = mm / 10 * 16 + mm % 10;
	*(((DB*)&header.A_BF_BPB_VolumeID) + 1) = hh / 10 * 16 + hh % 10;
	*(((DB*)&header.A_BF_BPB_VolumeID) + 0) = dd / 10 * 16 + dd % 10;

    // Setting number of hidden sectors
	header.A_BF_BPB_HiddenSectors = hiddenSectors;

    // Write the FAT32 Header and FSInfo to the filesystem buffer
	writeBinary(header, filesystem);
	addFSInfo(header, filesystem + 512);

    // Boot Signature
	filesystem[1534] = 0x55;
	filesystem[1535] = 0xAA;
}

// Reads the boot-loader code to a buffer
int readBootLoader(char* filename, unsigned char* buffer)
{
	std::ifstream in(filename, std::ios::binary);
    if(!in.good())
    {   return -1;
    }

	in.read((char*)buffer, 2*512);
	in.close();
    return 0;
}

// Adds a signature on the Master Boot Record
// Signature is used to identify media devices formatted by the utility
void addSignature(char* data, int idx, const char* sig, int length)
{
    for(int i=0, j=idx; i<length; i++, j++)
    {   data[j] = sig[i];
    }
}

// Checks if the DriveName is a valid physical device
// Checks if the drive is safe to be formatted by matching a signature
int safetyCheck(const char* driveName, int index, const char* str)
{
    char sector[512];
    if(readPhysicalDrive(driveName, 0, sector, 512) != 0)
    {   return -1;
    }

    for(int i=0, j=index; str[i] != 0; i++, j++)
    {   if(str[i]!= sector[j])
        {   return -2;
        }
    }

    return 0;
}

// Builds the Boot Code and the File System
// Returns -1 on fail, 0 on success
int build(const char* driveName, int driveNum)
{
	unsigned char data[13*512];
	memset(data, 0, 13*512);

	FAT32_HEADER diskpart;
	setVolumeName(diskpart, "Sore-OS");
	setPartitionSize(diskpart, 8388608);

    // Loading drive information in the file system
	if(setDriveInformation(diskpart, driveName) == -1)
    {   return -1;
    }

    // Reading the boot-loader code
	if(readBootLoader("boot-loader", data) == -1)
    {   return -1;
    }

    // Add boot partition and file system
	addPartition(diskpart, data);
    addSignature((char*)data, 0x1B6, "Sore-OS", 7);
	addFileSystem(diskpart, 1, data + (1*512));

    // Backup FAT32 Boot Sector
	memcpy(data+(7*512), data+(1*512), (3*512));

    // Get all the volumes on the physical drive
    char driveLetters[27];
    void* volumeHandles[27];
    findVolumes(driveNum, driveLetters, volumeHandles);

    // Lock the volumes that are on the drive
    if(lockVolumes(driveLetters, volumeHandles) != 0)
    {   unlockVolumes(driveLetters, volumeHandles);
        return -1;
    }

    // Write data to the physical disk
    if(writePhysicalDrive(driveName, 0, (char*)data, 13*512) != 0)
    {   return -1;
    }

    // Clear the File System
    if(clearStorage(driveName, diskpart) == -1)
    {   return -1;
    }

    // Unlock the volumes on the drive, previously locked
    unlockVolumes(driveLetters, volumeHandles);

    return 0;
}