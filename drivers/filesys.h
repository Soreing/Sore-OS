#ifndef FILESYS_H
#define FILESYS_H

#include <stdbool.h>
#include <Drivers/file.h>

#define FTYPE_DIRECTORY 1
#define FTYPE_FILE 2

// Loads the file system on a drive and opens the Root directory
// by traversing its cluster indexes and loading the first cluster
bool loadFileSystem();

// Finds a file in the current directory by name
// Returns the start cluster index of the file or -1 if not found
int findFile(const char* filename, char *type);

// Gets a File object from a start cluster
int getFile(const int startCluster, struct File* file);

// Prints the contents of the file on the screen
void printFile(struct File file);

// Lists files in the current directory
void listFiles();

// Returns the system path
char* getWorkingDirectory();

// Changes the current directory and adjusts the system path
void changeDirectory(char* dirName);

#endif