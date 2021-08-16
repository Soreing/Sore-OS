#include "DiskUtil.h"
#include <windows.h>
#include <iostream>

// Identifies the volumes with drive letters that belong to a specific 
// physical drive number. Letters are stored in "volumes", null terminated
void findVolumes(int driveNum, char letters[27], void* handles[27])
{
    HANDLE vol;
	STORAGE_DEVICE_NUMBER info;

	int  res;
	int  idx        = 0;
	char letter     = 'A';
	char volName[7] = "\\\\.\\A:";

	for (char letter = 'A'; letter <= 'Z'; letter++, volName[4]++)
	{	// Open the Volume
		vol = CreateFile(
			volName,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			NULL,
			NULL
		);

		if (vol != INVALID_HANDLE_VALUE)
		{	// If the volume opened, get its device number
			res = DeviceIoControl(
				vol,
				IOCTL_STORAGE_GET_DEVICE_NUMBER,
				NULL, 0,
				&info, sizeof(info),
				NULL,
				NULL
			);

			// If the device number is retrieved and matches the seatch,
			// add it to the volumes list and increment the index
			if (res != 0 && info.DeviceNumber == driveNum)
			{	letters[idx] = letter;
                handles[idx] = vol;
				idx++;
			}
            else
            {   CloseHandle(vol);
            }
		}
	}

	letters[idx] = 0;
}

// Dismounts the volumes listed in letters with their corresponding handles
// Letters is null terminated to mark the end of the structure
int lockVolumes(char letters[27], void* handles[27])
{
    int res;
    for(int i=0; letters[i]!=0; i++)
    {
        res = DeviceIoControl(
            handles[i],
            FSCTL_LOCK_VOLUME,
            NULL, 0,
            NULL, 0,
            NULL,
            NULL
        );

        if(res == 0)
        {   return -1;
        }

        res = DeviceIoControl(
            handles[i],
            FSCTL_DISMOUNT_VOLUME,
            NULL, 0,
            NULL, 0,
            NULL,
            NULL
        );

        if(res == 0)
        {   return -1;
        }
    }

    return 0;
}

// Closes the volumes listed in letters with their corresponding handles
// Letters is null terminated to mark the end of the structure
void unlockVolumes(char letters[27], void* handles[27])
{
    for(int i=0; letters[i]!=0; i++)
    {   CloseHandle(handles[i]);
    }
}

// Reads data from a Physical Drive from a sepcific LBA sector
// "size" amount of data is read and stored in the buffer
int readPhysicalDrive(const char* driveName, int LBA, char* buffer, int size)
{
    int res;
    DWORD dwRead;

    HANDLE hdevice = CreateFileA(
        driveName,
        GENERIC_READ,
        NULL,
        NULL,
        OPEN_EXISTING,
        NULL,
        NULL
    );

    if (hdevice == INVALID_HANDLE_VALUE)
    {   return -1;
    }

    SetFilePointer(hdevice, LBA*512, 0, FILE_BEGIN);
    res = ReadFile(hdevice, buffer, size, &dwRead, NULL);

    if (res == 0)
    {   CloseHandle(hdevice);
        return -1;
    }

    CloseHandle(hdevice);
    return 0;
}

// Writes data to a Physical Drive from a sepcific LBA sector
// "size" amount of data is written from buffer to the drive
int writePhysicalDrive(const char* driveName, int LBA, char* buffer, int size)
{
    int res;
    DWORD dwWrite;

    HANDLE hdevice = CreateFileA(
        driveName,
        GENERIC_WRITE,
        NULL,
        NULL,
        OPEN_EXISTING,
        NULL,
        NULL
    );

    if (hdevice == INVALID_HANDLE_VALUE)
    {   std::cout<< "Fail 1\n";
        return -1;
    }

    SetFilePointer(hdevice, LBA*512, 0, FILE_BEGIN);
    res = WriteFile(hdevice, buffer, size, &dwWrite, NULL);

    if (res == 0)
    {   std::cout<< "Fail 2\n";
        std::cout<< GetLastError() << "\n";
        CloseHandle(hdevice);
        return -1;
    }

    CloseHandle(hdevice);
    return 0;
}