#include <kernel/asm.h>

unsigned char inb(unsigned short port) 
{
	// A handy C wrapper function that reads a byte from the specified port
	//  "=a" (result) means: put AL register in variable RESULT when finished
	//  "d" (port) means: load EDX with port
	unsigned char result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}
	
void outb(unsigned short port , unsigned char data) 
{
	// "a" (data) means: load EAX with data
	// "d" (port) means: load EDX with port
	__asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

unsigned short inw(unsigned short port) 
{
	unsigned short result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}
	
void outw(unsigned short port , unsigned short data) 
{
	__asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
}