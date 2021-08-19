#include <drivers/iobuffers.h>
#include <drivers/screen.h>

char reading=0;

int tempIndex=0;
char tempBuffer[2048];

int inputIndex=0;
char inputBuffer[2048];

int outputIndex=0;
char outputBuffer[2048];

// Removes some characters and white spaces from the input buffer
// At least "count" number of characters are removed, assumed consumed
void trimInput(int count)
{
    while(inputBuffer[count] == ' ' || inputBuffer[count] == '\n')
    {   count++;
    }

    for(int dsti=0, srci = count; srci < inputIndex ; dsti++, srci++)
    {   inputBuffer[dsti] = inputBuffer[srci];
    }

    inputIndex -= count;
}

// Waits for input and extracts characters from the inputBuffer
// till the delimiter character or the end if the buffer
void getLine(char* buffer, int size, char delim)
{
    // Waiting for Input
    reading=1;
    while(inputIndex==0) {}
    reading=0;

    // Copying data from the input buffer to the buffer
    int idx;
    for(idx=0; inputBuffer[idx]!=delim && idx < size-1; idx++)
    {   buffer[idx] = inputBuffer[idx];
    }

    buffer[idx]=0;
    trimInput(idx);
}

// Waits for input and extracts characters from the inputBuffer
void getStr(char* buffer, int size)
{
    // Waiting for Input
    reading=1;
    while(inputIndex==0) {}
    reading=0;

    // Copying data from the input buffer to the buffer
    // Copying stops at a space or a newline or the end of the buffer
    int idx;
    for(idx=0; inputBuffer[idx]!=' ' && inputBuffer[idx]!='\n' && idx < size-1; idx++)
    {   buffer[idx] = inputBuffer[idx];
    }

    buffer[idx]=0;
    trimInput(idx);
}

// Prints a string to the screen unbuffered
void printStrZ(char* buffer)
{
    for(int i=0; buffer[i] != 0; i++)
    {   putChar(buffer[i]);
    }
}

void printStr(char* buffer, int length)
{
    for(int i=0; i < length; i++)
    {   putChar(buffer[i]);
    }
}

// Prints a character to the screen unbuffered
void printChar(char ch)
{
    putChar(ch);
}

// Prints a stream of bytes to the screen as hexadecimal
void printHex(unsigned char* start, int bytes)
{
    static char const hex[16] ={
        '0','1','2','3','4','5','6','7',
        '8','9','A','B','C','D','E','F',
    };

    for(int i=0; i<bytes ; i++)
    {   putChar(hex[(start[i]>>4)]);
        putChar(hex[(start[i]&0x0F)]);
    }
}

//Clears the Input Buffer
void clearInputBuffer()
{   inputIndex=0;
}