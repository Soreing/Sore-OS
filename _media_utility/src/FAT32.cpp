#include "FAT32.h"
#include "DiskUtil.h"
#include <windows.h>
#include <string.h>
#include <iostream>

// Sets the Volume Name to some given name
// The size is always 11 characters, padded by spaces
void setVolumeName(FAT32_HEADER &hdr, const char* name)
{
    bool end = false;
    for(int idx = 0; idx < 11; idx++)
    {
        if(!end && name[idx] == 0)
        {   end = true;
        }
        
        hdr.A_BF_BPB_VolumeLabel[idx] = end ? ' ' : name[idx];
    }
}

// Sets the number of sectors in the partition
// Calculates the sectors it takes to store the FAT
void setPartitionSize(FAT32_HEADER &hdr, int sectors)
{
    hdr.A_BF_BPB_BigTotalSectors = sectors;
    hdr.A_BF_BPB_BigSectorsPerFat = (sectors / hdr.A_BF_BPB_SectorsPerCluster / 128) + 1;
    hdr.LBA_FAT_Location = 1 + hdr.A_BF_BPB_ReservedSectors;
    hdr.LBA_Cluster_Location = hdr.LBA_FAT_Location + 2*hdr.A_BF_BPB_BigSectorsPerFat;
}

// Opens a Physical Drive and gets the heads/track information out of it
// Returns 0 on success, or an error value otherwise
int setDriveInformation(FAT32_HEADER &hdr, const char* driveName)
{	
    _OVERLAPPED ovlStr;
    _DISK_GEOMETRY disk;

    HANDLE sig = CreateEventA(NULL, TRUE, FALSE, NULL);
    memset(&ovlStr, 0, sizeof(ovlStr));
    ovlStr.hEvent = sig;

    HANDLE hdevice = CreateFileA(
        driveName,
        GENERIC_READ,
        NULL,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hdevice <= 0)
    {    return -1;
    }

    int ret = DeviceIoControl(
        (HANDLE)hdevice,
        IOCTL_DISK_GET_DRIVE_GEOMETRY,
        NULL,
        0,
        (LPVOID)&disk,
        (DWORD)sizeof(struct _DISK_GEOMETRY),
        (LPDWORD)NULL,
        (LPOVERLAPPED)&ovlStr
    );

    if (ret == 0)
    {   return -1;
        CloseHandle(hdevice);
    }

    WaitForSingleObject(ovlStr.hEvent, INFINITE);
    hdr.A_BF_BPB_Heads = (DW)disk.TracksPerCylinder;
    hdr.A_BF_BPB_SectorsPerTrack = (DW)disk.SectorsPerTrack;
    
    CloseHandle(hdevice);
    return 0;
}

int clearStorage(const char* driveName, const FAT32_HEADER &header)
{
    char FAT[512];
    char Root[2048];

    int FAT1_begin    = header.LBA_FAT_Location;
    int FAT2_begin    = header.A_BF_BPB_BigSectorsPerFat + FAT1_begin;
    int Cluster_begin = header.LBA_Cluster_Location;

    memset(&FAT,  0, 512);
    memset(&Root, 0, 2048);

    *(DD*)(FAT + 0x00) = 0x0FFFFFF8;
    *(DD*)(FAT + 0x04) = 0xFFFFFFFF;
    *(DD*)(FAT + 0x08) = 0x0FFFFFFF;

    if(writePhysicalDrive(driveName, FAT1_begin, FAT, 512) != 0 )
    {   return -1;
    }

    if(writePhysicalDrive(driveName, FAT2_begin, FAT, 512) != 0 )
    {   return -1;
    }

    if(writePhysicalDrive(driveName, Cluster_begin, Root, 2048) != 0 )
    {   return -1;
    }

    return 0;
}

// Loads the FAT32 HEADER information to a buffer
// Includes a signature and a jump instruction to code
// Assumes that the buffer is cleared and  set to 0s
void writeBinary(const FAT32_HEADER &hdr, unsigned char buffer[512])
{
    buffer[0x00] = 0xEB;
    buffer[0x01] = 0x64;
    buffer[0x02] = 0x90;

    for(int idx=0, off= 0x03; idx<8; idx++, off++)
        buffer[off] = hdr.OEM_NAME[idx];

    *(DW*)(buffer + 0x0B) = hdr.A_BF_BPB_BytesPerSector;
    *(DB*)(buffer + 0x0D) = hdr.A_BF_BPB_SectorsPerCluster;
    *(DW*)(buffer + 0x0E) = hdr.A_BF_BPB_ReservedSectors;
    *(DB*)(buffer + 0x10) = hdr.A_BF_BPB_NumberOfFATs;
    *(DW*)(buffer + 0x11) = hdr.A_BF_BPB_RootEntries;
    *(DW*)(buffer + 0x13) = hdr.A_BF_BPB_TotalSectors;
    *(DB*)(buffer + 0x15) = hdr.A_BF_BPB_MediaDescriptor;
    *(DW*)(buffer + 0x16) = hdr.A_BF_BPB_SectorsPerFAT;
    *(DW*)(buffer + 0x18) = hdr.A_BF_BPB_SectorsPerTrack;
    *(DW*)(buffer + 0x1A) = hdr.A_BF_BPB_Heads;
    *(DD*)(buffer + 0x1C) = hdr.A_BF_BPB_HiddenSectors;
    *(DD*)(buffer + 0x20) = hdr.A_BF_BPB_BigTotalSectors;
    *(DD*)(buffer + 0x24) = hdr.A_BF_BPB_BigSectorsPerFat;
    *(DW*)(buffer + 0x28) = hdr.A_BF_BPB_ExtFlags;
    *(DW*)(buffer + 0x2A) = hdr.A_BF_BPB_FS_Version;
    *(DD*)(buffer + 0x2C) = hdr.A_BF_BPB_RootDirStrtClus;
    *(DW*)(buffer + 0x30) = hdr.A_BF_BPB_FSInfoSec;
    *(DW*)(buffer + 0x32) = hdr.A_BF_BPB_BkUpBootSec;
    *(DW*)(buffer + 0x34) = hdr.A_BF_BPB_Reserved1;
    *(DW*)(buffer + 0x38) = hdr.A_BF_BPB_Reserved2;
    *(DW*)(buffer + 0x3C) = hdr.A_BF_BPB_Reserved3;
    *(DB*)(buffer + 0x40) = hdr.A_BF_BPB_DriveNum;
    *(DB*)(buffer + 0x41) = hdr.A_BF_BPB_Reserved4;
    *(DB*)(buffer + 0x42) = hdr.A_BF_BPB_Signature;
    *(DD*)(buffer + 0x43) = hdr.A_BF_BPB_VolumeID;

    for (int idx = 0, off = 0x47; idx<11; idx++, off++)
        buffer[off] = hdr.A_BF_BPB_VolumeLabel[idx];

    for (int idx = 0, off = 0x52; idx<8; idx++, off++)
        buffer[off] = hdr.A_BF_BPB_FAT_Name[idx];

    *(DD*)(buffer + 0x5A) = hdr.LBA_FAT_Location;
    *(DD*)(buffer + 0x5E) = hdr.LBA_Cluster_Location;
    *(DB*)(buffer + 0x62) = hdr.Scale_SectorsPerCluster;
    *(DB*)(buffer + 0x63) = hdr.Mask_SectorRotation;

    buffer[0x1FE] = 0x55;
    buffer[0x1FF] = 0xAA;
}
