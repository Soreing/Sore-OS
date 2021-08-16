#ifndef FAT32_HEADER_CLASS
#define FAT32_HEADER_CLASS

typedef unsigned short DW;
typedef unsigned char  DB;
typedef unsigned long  DD;

struct FAT32_HEADER
{
	DB OEM_NAME [8]               = {' '};	// 0x 03  -  Name of the System formatting the partition
	
	DW A_BF_BPB_BytesPerSector    = 512;	// 0x 0B  -  Always 512 Bytes
	DB A_BF_BPB_SectorsPerCluster = 4;		// 0x 0D  -  1:512B, 2:1KB, 4:2KB ... 
	DW A_BF_BPB_ReservedSectors   = 12;		// 0x 0E  -  Number of sectors reserved for the Boot Code on a Partition
	DB A_BF_BPB_NumberOfFATs      = 2;		// 0x 10  -  Number of File Allocation Tables (FAT)
	DW A_BF_BPB_RootEntries	      = 0;		// 0x 11  -  Must be 0 for FAT32
	DW A_BF_BPB_TotalSectors      = 0;		// 0x 13  -  Must be 0 for FAT32
	DB A_BF_BPB_MediaDescriptor   = 0xF8;	// 0x 15  -  Media Type 0xF8 for hard drive, 
	DW A_BF_BPB_SectorsPerFAT     = 0;		// 0x 16  -  Must be 0 for FAT32
	DW A_BF_BPB_SectorsPerTrack   = 0;		// 0x 18  -  (TO BE FILLED IN) Drive specific number of sectors per track
	DW A_BF_BPB_Heads             = 0;		// 0x 1A  -  (TO BE FILLED IN) Drive specific number of tracks
	DD A_BF_BPB_HiddenSectors     = 0;		// 0x 1C  -  (TO BE FILLED IN) Offset of the Partition
	DD A_BF_BPB_BigTotalSectors   = 0;		// 0x 20  -  (TO BE FILLED IN) Sectors in the Partition
	DD A_BF_BPB_BigSectorsPerFat  = 0;		// 0x 24  -  (TO BE FILLED IN) Sector size of each FAT
	DW A_BF_BPB_ExtFlags          = 0;		// 0x 28  -  Extended flags, 0 is fine
	DW A_BF_BPB_FS_Version        = 0;		// 0x 2A  -  Major Revision Number (Fine as 0?)
	DD A_BF_BPB_RootDirStrtClus   = 2;		// 0x 2C  -  Cluster of the Root Directory (Usually 2, not allowed to be lower)
	DW A_BF_BPB_FSInfoSec         = 1;		// 0x 30  -  File System Info Sector (Usually 1)
	DW A_BF_BPB_BkUpBootSec       = 6;		// 0x 32  -  Location (From the Partition) of the Backup Boot Sector (0 means no backup)
	DW A_BF_BPB_Reserved1         = 0;		// 
	DW A_BF_BPB_Reserved2         = 0;		// 0x 34  -  12 Bytes Reserved Space
	DW A_BF_BPB_Reserved3         = 0;		//
	DB A_BF_BPB_DriveNum          = 0x80;	// 0x 40  -  Logical Drive Number of the Partition (First Hard Drive?)
	DB A_BF_BPB_Reserved4         = 0;		// 0x 41  -  1 Byte Reserved Space
	DB A_BF_BPB_Signature         = 0x29;	// 0x 42  -  Extended Signature (Always 0x29)
	DD A_BF_BPB_VolumeID          = 0;		// 0x 43  -  (TO BE FILLED IN) Timestamp of creation
	DB A_BF_BPB_VolumeLabel[11]   = {' '};	// 0x 47  -  Volume name of the Partition
	DB A_BF_BPB_FAT_Name[8]       = { 'F','A', 'T', '3', '2', ' ', ' ', ' ' };
	// Additional Boot Details
	DD LBA_FAT_Location           = 0;		// 0x 5A  -  LBA location of the First FAT Region
	DD LBA_Cluster_Location       = 0;		// 0x 5E  -  LBA location of the cluster region 
	DB Scale_SectorsPerCluster    = 2;		// 0x 62  -  Bit shift scale of Sectors per Cluster
	DB Mask_SectorRotation        = 3;		// 0x 63  -  Bit rotation mask to transfer bits after multiplication
	// Record Ends at Byte 0x64
};

// Sets the Volume Name to some given name
// The size is always 11 characters, padded by spaces
void setVolumeName(FAT32_HEADER &hdr, const char* name);

// Sets the number of sectors in the partition
// Calculates the sectors it takes to store the FAT
void setPartitionSize(FAT32_HEADER &hdr, int sectors);

// Opens a Physical Drive and gets the heads/track information out of it
// Returns 0 on success, or an error value otherwise
int setDriveInformation(FAT32_HEADER &hdr, const char* driveName);

// Sets the FAT cluster indexes to default values and empties the Root folder
int clearStorage(const char* driveName, const FAT32_HEADER &header);

// Loads the FAT32 HEADER information to a buffer
// Includes a signature and a jump instruction to code
// Assumes that the buffer is cleared and  set to 0s
void writeBinary(const FAT32_HEADER &hdr, unsigned char buffer[512]);

#endif