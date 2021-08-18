#ifndef FAT32_HEADER_CLASS
#define FAT32_HEADER_CLASS

typedef unsigned short DW;
typedef unsigned char  DB;
typedef unsigned long  DD;

struct FAT32_HEADER
{
	DB OEM_NAME [8];				// 0x 03  -  Name of the System formatting the partition
	
	DW A_BF_BPB_BytesPerSector;		// 0x 0B  -  Always 512 Bytes
	DB A_BF_BPB_SectorsPerCluster;	// 0x 0D  -  1:512B, 2:1KB, 4:2KB ... 
	DW A_BF_BPB_ReservedSectors;	// 0x 0E  -  Number of sectors reserved for the Boot Code on a Partition
	DB A_BF_BPB_NumberOfFATs;		// 0x 10  -  Number of File Allocation Tables (FAT)
	DW A_BF_BPB_RootEntries;		// 0x 11  -  Must be 0 for FAT32
	DW A_BF_BPB_TotalSectors;		// 0x 13  -  Must be 0 for FAT32
	DB A_BF_BPB_MediaDescriptor;	// 0x 15  -  Media Type 0xF8 for hard drive, 
	DW A_BF_BPB_SectorsPerFAT;		// 0x 16  -  Must be 0 for FAT32
	DW A_BF_BPB_SectorsPerTrack;	// 0x 18  -  (TO BE FILLED IN) Drive specific number of sectors per track
	DW A_BF_BPB_Heads;				// 0x 1A  -  (TO BE FILLED IN) Drive specific number of tracks
	DD A_BF_BPB_HiddenSectors;		// 0x 1C  -  (TO BE FILLED IN) Offset of the Partition
	DD A_BF_BPB_BigTotalSectors;	// 0x 20  -  (TO BE FILLED IN) Sectors in the Partition
	DD A_BF_BPB_BigSectorsPerFat;	// 0x 24  -  (TO BE FILLED IN) Sector size of each FAT
	DW A_BF_BPB_ExtFlags;			// 0x 28  -  Extended flags, 0 is fine
	DW A_BF_BPB_FS_Version;			// 0x 2A  -  Major Revision Number (Fine as 0?)
	DD A_BF_BPB_RootDirStrtClus;	// 0x 2C  -  Cluster of the Root Directory (Usually 2, not allowed to be lower)
	DW A_BF_BPB_FSInfoSec;			// 0x 30  -  File System Info Sector (Usually 1)
	DW A_BF_BPB_BkUpBootSec;		// 0x 32  -  Location (From the Partition) of the Backup Boot Sector (0 means no backup)
	DW A_BF_BPB_Reserved1;			// 
	DW A_BF_BPB_Reserved2;			// 0x 34  -  12 Bytes Reserved Space
	DW A_BF_BPB_Reserved3;			//
	DB A_BF_BPB_DriveNum;			// 0x 40  -  Logical Drive Number of the Partition (First Hard Drive?)
	DB A_BF_BPB_Reserved4;			// 0x 41  -  1 Byte Reserved Space
	DB A_BF_BPB_Signature;			// 0x 42  -  Extended Signature (Always 0x29)
	DD A_BF_BPB_VolumeID;			// 0x 43  -  (TO BE FILLED IN) Timestamp of creation
	DB A_BF_BPB_VolumeLabel[11];	// 0x 47  -  Volume name of the Partition
	DB A_BF_BPB_FAT_Name[8];
	// Additional Boot Details
	DD LBA_FAT_Location;			// 0x 5A  -  LBA location of the First FAT Region
	DD LBA_Cluster_Location;		// 0x 5E  -  LBA location of the cluster region 
	DB Scale_SectorsPerCluster;		// 0x 62  -  Bit shift scale of Sectors per Cluster
	DB Mask_SectorRotation;			// 0x 63  -  Bit rotation mask to transfer bits after multiplication
	// Record Ends at Byte 0x64
};

// Reads the FAT32 HEADER information from a buffer
void readHeader(struct FAT32_HEADER *hdr, const unsigned char buffer[512]);

#endif