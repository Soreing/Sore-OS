#ifndef FILESYS_H
#define FILESYS_H

#include <stdbool.h>

#define FTYPE_DIRECTORY 1
#define FTYPE_FILE 2

// Loads the file system on a drive and opens the Root directory
// by traversing its cluster indexes and loading the first cluster
bool loadFileSystem();

// Finds a file in the current directory by name
// Returns the start cluster index of the file or -1 if not found
int findFile(const char* filename, char *type);

#endif