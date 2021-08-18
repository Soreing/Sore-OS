#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int   memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* dst, const void* src, size_t size);
void* memset(void* ptr, int val, size_t size);

int    strcmp(const char* astr, const char* bstr);
char*  strcpy(char* dst, const char* src);
size_t strlen(const char* str);

#endif
