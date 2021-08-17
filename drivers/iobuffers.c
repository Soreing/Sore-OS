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

// Puts a string to the screen unbuffered
void putStr(char* buffer)
{
    for(int i=0; buffer[i] != 0; i++)
    {   putChar(buffer[i]);
    }
}