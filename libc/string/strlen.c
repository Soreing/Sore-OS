#include <string.h>

size_t strlen(const char* str)
{
    size_t count;
	for (count = 0; str[count]!=0; count++){};

    return count;
}