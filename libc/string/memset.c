#include <string.h>

void* memset(void* ptr, int val, size_t size) 
{
	unsigned char* a = (unsigned char*) ptr;

	for (size_t i = 0; i < size; i++) 
    {   a[i] = val;
	}

    return ptr;
}