#include <string.h>

void* memcpy(void* dst, const void* src, size_t size) 
{
	unsigned char* a = (unsigned char*) dst;
	const unsigned char* b = (const unsigned char*) src;

	for (size_t i = 0; i < size; i++) 
    {   a[i] = b[i];
	}

    return dst;
}