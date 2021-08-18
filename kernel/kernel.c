#include <kernel/asm.h>
#include <kernel/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>
#include <drivers/filesys.h>

void changeDirCMD()
{
	char directoryName[256];
	getLine(directoryName, 256, '\n');

	printStr("This command doesn't exist yet, but you entered: ");
	printStr(directoryName);
	printStr("\n");
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
		printChar('>');

		getStr(buffer, 256);

		if(strcmp(buffer, "cd") == 0)
		{	changeDirCMD();
		}
	}

	char type;
	int  cluster = findFile("My New Directory", &type);
	printStr("Cluster: ");
	printHex((unsigned char*)&cluster, 4);
	printStr("\nType:    ");
	printHex((unsigned char*)&type, 1);
}
