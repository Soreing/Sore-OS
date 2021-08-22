#include <kernel/asm.h>
#include <kernel/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>
#include <drivers/filesys.h>
#include <drivers/file.h>
#include <string.h>


// Changes the current directory and adjusts the system path
void changeDirCMD()
{
    struct File file;
	char directoryName[256];

	// Get Directory Name from the user
	getLine(directoryName, 256, '\n');

	// Find if an entry exists with this name in the current directory
    if(findFile(directoryName, &file) == 0)
	{	// If the entry is of a directory type, change directory
		if(file.type == FTYPE_DIRECTORY)
		{	if(openDirectory(file))
			{   // Adjust the system path with the directory change
				if(strcmp(directoryName, "..") == 0)
				{   delPath();
				}
				else if(strcmp(directoryName, ".") != 0)
				{   addPath(directoryName);
				}
			}
		}
		else
		{	printStrZ(directoryName);
			printStrZ(": Not a directory\n");
		}
	}
	else
	{	printStrZ(directoryName);
		printStrZ(": No such file or directory\n");	
	}
}

void printFileCMD()
{
	struct File file;
	char fileName[256];

	// Get File Name from the user
	getLine(fileName, 256, '\n');

	// Find if an entry exists with this name in the current directory
	if(findFile(fileName, &file) == 0)
	{	// If the entry is of a file type, print the file
		if(file.type == FTYPE_FILE)
		{	printFile(file);
			printChar('\n');
		}
		else
		{	printStrZ(fileName);
			printStrZ(": Is a directory\n");
		}
	}
	else
	{	printStrZ(fileName);
		printStrZ(": No such file or directory\n");
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
