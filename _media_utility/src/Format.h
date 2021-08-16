#ifndef FORMAT_H
#define FORMAT_H

#include "FAT32.h"

// Adds a partition to the buffer
// Uses the details from the file system using a FAT32 Header
// Partitiontable is assumed to be cleared and filled with 0s
void addPartition(const FAT32_HEADER &header, unsigned char* partitiontable);

// Adds an FSInfo structure to the buffer
void addFSInfo(const FAT32_HEADER &header, unsigned char* buffer);

// Adds a FAT32 file system to the buffer
// Filesystem is assumed to be cleared and filled with 0s
void addFileSystem(FAT32_HEADER &header, int hiddenSectors, unsigned char* filesystem);

// Reads the boot-loader code to a buffer
// Returns -1 on fail, 0 on success
int readBootLoader(char* filename, unsigned char* buffer);

// Adds a signature on the Master Boot Record
// Signature is used to identify media devices formatted by the utility
void addSignature(char* data, int idx, const char* sig, int length);

// Checks if the DriveName is a valid physical device
// Checks if the drive is safe to be formatted by matching a signature
// Returns -1 on fail, -2 on unsafe, 0 on safe device.
int safetyCheck(const char* driveName, int index, const char* str);

// Builds the Boot Code and the File System
// Returns -1 on fail, 0 on success
int build(const char* driveName, int driveNum);

#endif