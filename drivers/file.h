#ifndef FILE_H
#define FILE_H

struct File
{   char type;
    int  size;
    int  clusters[8];
};

#endif