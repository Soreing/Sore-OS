#ifndef FILESYS_H
#define FILESYS_H

#include <stdbool.h>

// Loads the file system on a drive and opens the Root directory
// by traversing its cluster indexes and loading the first cluster
bool loadFileSystem();

#endif