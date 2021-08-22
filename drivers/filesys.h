#ifndef FILESYS_H
#define FILESYS_H

#include <stdbool.h>
#include <Drivers/file.h>

#define FTYPE_DIRECTORY 1
#define FTYPE_FILE 2

// Loads the file system on a drive and opens the Root directory
// by traversing its cluster indexes and loading the first cluster
bool loadFileSystem();

// Finds a file in the current directory by name and fills the file structure
// Returns 0 on success, -1 on error
int findFile(const char* filename, struct File *type);

// Prints the contents of the file on the screen
void printFile(struct File file);

// Lists files in the current directory
void listFiles();

// Returns the system path
char* getWorkingDirectory();

// Replace the current directory's clusters with a new one
// Reads the first cluster and sets the directory cluster index to 0
bool openDirectory(struct File directory);

// Adds a new directory name to the system path
void addPath(char* name);

// Removes the last directory name from the system path
void delPath();

#endif