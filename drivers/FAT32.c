#include <drivers/FAT32.h>
#include <string.h>

// Reads the FAT32 HEADER information from a buffer
void readHeader(struct FAT32_HEADER *hdr,  const unsigned char buffer[512])
{
    memcpy(hdr->OEM_NAME, buffer+0x03, 8);

    hdr->A_BF_BPB_BytesPerSector    = *(DW*)(buffer + 0x0B);
    hdr->A_BF_BPB_SectorsPerCluster = *(DB*)(buffer + 0x0D);
    hdr->A_BF_BPB_ReservedSectors   = *(DW*)(buffer + 0x0E);
    hdr->A_BF_BPB_NumberOfFATs      = *(DB*)(buffer + 0x10);
    hdr->A_BF_BPB_RootEntries       = *(DW*)(buffer + 0x11);
    hdr->A_BF_BPB_TotalSectors      = *(DW*)(buffer + 0x13);
    hdr->A_BF_BPB_MediaDescriptor   = *(DB*)(buffer + 0x15);
    hdr->A_BF_BPB_SectorsPerFAT     = *(DW*)(buffer + 0x16);
    hdr->A_BF_BPB_SectorsPerTrack   = *(DW*)(buffer + 0x18);
    hdr->A_BF_BPB_Heads             = *(DW*)(buffer + 0x1A);
    hdr->A_BF_BPB_HiddenSectors     = *(DD*)(buffer + 0x1C);
    hdr->A_BF_BPB_BigTotalSectors   = *(DD*)(buffer + 0x20);
    hdr->A_BF_BPB_BigSectorsPerFat  = *(DD*)(buffer + 0x24);
    hdr->A_BF_BPB_ExtFlags          = *(DW*)(buffer + 0x28);
    hdr->A_BF_BPB_FS_Version        = *(DW*)(buffer + 0x2A);
    hdr->A_BF_BPB_RootDirStrtClus   = *(DD*)(buffer + 0x2C);
    hdr->A_BF_BPB_FSInfoSec         = *(DW*)(buffer + 0x30);
    hdr->A_BF_BPB_BkUpBootSec       = *(DW*)(buffer + 0x32);
    hdr->A_BF_BPB_Reserved1         = *(DW*)(buffer + 0x34);
    hdr->A_BF_BPB_Reserved2         = *(DW*)(buffer + 0x38);
    hdr->A_BF_BPB_Reserved3         = *(DW*)(buffer + 0x3C);
    hdr->A_BF_BPB_DriveNum          = *(DB*)(buffer + 0x40);
    hdr->A_BF_BPB_Reserved4         = *(DB*)(buffer + 0x41);
    hdr->A_BF_BPB_Signature         = *(DB*)(buffer + 0x42);
    hdr->A_BF_BPB_VolumeID          = *(DD*)(buffer + 0x43);

    memcpy(hdr->A_BF_BPB_VolumeLabel, buffer+0x47, 11);
    memcpy(hdr->A_BF_BPB_FAT_Name,    buffer+0x52, 8);

    hdr->LBA_FAT_Location        = *(DD*)(buffer + 0x5A);
    hdr->LBA_Cluster_Location    = *(DD*)(buffer + 0x5E);
    hdr->Scale_SectorsPerCluster = *(DB*)(buffer + 0x62);
    hdr->Mask_SectorRotation     = *(DB*)(buffer + 0x63);
}
