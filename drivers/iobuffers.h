#ifndef IOBUFFERS_H
#define IOBUFFERS_H

extern char reading;

extern int tempIndex;
extern char tempBuffer[2048];

extern int inputIndex;
extern char inputBuffer[2048];

extern int outputIndex;
extern char outputBuffer[2048];

void getLine(char* buffer, int size, char delim);
void getStr(char* buffer, int size);

void printStrZ(char* buffer);
void printStr(char* buffer, int length);
void printChar(char ch);
void printHex(unsigned char* start, int bytes);

void clearInputBuffer();

#endif