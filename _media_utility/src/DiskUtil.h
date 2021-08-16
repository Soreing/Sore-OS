#ifndef DISKUTIL_H
#define DISKUTIL_H

// Identifies the volumes with drive letters that belong to a specific 
// physical drive number. Letters are stored in "volumes", null terminated
void findVolumes(int driveNum, char letters[27], void* handles[27]);

// Dismounts the volumes listed in letters with their corresponding handles
// Letters is null terminated to mark the end of the structure
int lockVolumes(char letters[27], void* handles[27]);

// Closes the volumes listed in letters with their corresponding handles
// Letters is null terminated to mark the end of the structure
void unlockVolumes(char letters[27], void* handles[27]);

// Reads data from a Physical Drive from a sepcific LBA sector
// "size" amount of data is read and stored in the buffer
int readPhysicalDrive(const char* driveName, int LBA, char* buffer, int size);

// Writes data to a Physical Drive from a sepcific LBA sector
// "size" amount of data is written from buffer to the drive
int writePhysicalDrive(const char* driveName, int LBA, char* buffer, int size);

#endif