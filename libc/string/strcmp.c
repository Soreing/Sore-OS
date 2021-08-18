#include <string.h>

int strcmp(const char* astr, const char* bstr)
{
    for (size_t i=0; astr[i]!=0 || bstr[i]!=0; i++) 
    {   if (astr[i] < bstr[i])
		{	return -1;
        }
		else if (bstr[i] < astr[i])
        {   return 1;
        }
	}
    
	return 0;
}