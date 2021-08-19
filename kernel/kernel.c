#include <kernel/asm.h>
#include <kernel/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>
#include <drivers/filesys.h>
#include <drivers/file.h>
#include <string.h>

void changeDirCMD()
{
	char directoryName[256];
	getLine(directoryName, 256, '\n');
	changeDirectory(directoryName);
}

void printFileCMD()
{
	char type;
	int  cluster;
	char fileName[256];

	getLine(fileName, 256, '\n');
	cluster = findFile(fileName, &type);

	if(type == FTYPE_FILE)
	{	struct File file;
		getFile(cluster, &file);
		printFile(file);
		printChar('\n');
	}
	else
	{	printStrZ(fileName);
		printStrZ(": Is a directory\n");
	}
}


void main()
{
	idt_init();
	loadFileSystem();
	clearScreen();

	char buffer[256];
	while (true)
	{
		// Print working directory and $
		setFontColor(COLOR_LIGHT_GREEN);
		printStrZ(getWorkingDirectory());
		setFontColor(COLOR_WHITE);
		printStrZ("$ ");

		// Get a command from the user
		clearInputBuffer();
		getStr(buffer, 256);

		// Pick between valid commands
		if(strcmp(buffer, "cd") == 0)
		{	changeDirCMD();
		}
		else if(strcmp(buffer, "cat") == 0)
		{	printFileCMD();
		}
		else if(strcmp(buffer, "ls") == 0)
		{	listFiles();
		}
		else if(strcmp(buffer, "clear") == 0)
		{	clearScreen();
		}
	}
}
