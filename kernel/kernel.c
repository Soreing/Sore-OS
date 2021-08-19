#include <kernel/asm.h>
#include <kernel/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>
#include <drivers/filesys.h>
#include <string.h>

void changeDirCMD()
{
	char directoryName[256];
	getLine(directoryName, 256, '\n');
	changeDirectory(directoryName);
}

void listDirCMD()
{
	listFiles();
}

void main()
{
	idt_init();
	loadFileSystem();
	clearScreen();

	char buffer[256];
	while (true)
	{
		printStr(getWorkingDirectory());
		printChar('$ ');

		clearInputBuffer();
		getStr(buffer, 256);

		if(strcmp(buffer, "cd") == 0)
		{	changeDirCMD();
		}
		if(strcmp(buffer, "ls") == 0)
		{	listDirCMD();
		}
	}
}
