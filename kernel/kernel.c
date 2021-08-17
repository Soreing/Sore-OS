#include <kernel/asm.h>
#include <kernel/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>

#include <stdbool.h>

// Identifies if a drive exists and selects it
bool identify()
{
	char status=-1;
	char buffer[512];

	outb(0x1F6, 0xA0);	// Set Master Drive (0xA0) or Slave Drive (0xB0)
	outb(0x1F2, 0x00);	// Set Sector count to 0
	outb(0x1F3, 0x00);	// Set LBA lo to 0
	outb(0x1F4, 0x00);	// Set LBA mid to 0
	outb(0x1F5, 0x00);	// Set LBA hi to 0

	outb(0x1F7, 0xEC);	// Issue IDENTIFY command
	status=inb(0x1F7);	// Wait and Read status

	// If Status is 0, the drive does not exist
	if(status == 0)
	{	return false;
	}

	// Keep polling untill BSY is cleared
	while ((status & 0x80) != 0)
	{	status = inb(0x1F7);
	}

	// If LBAmid and LBAhi are non zero, not an ATA drive
	if (inb(0x1F4)!=0 || inb(0x1F5)!=0)
	{	return false;
	}

	// Keep polling untill DRQ or ERR are set
	while ((status & 0x09) == 0);
	{	status = inb(0x1F7);
	}
	
	// If ERR is clear, read 512 bytes (256 words)
	if((status & 0x01) == 0)
	{	for(long i=0; i<512; i+=2)
		{	*(short*)(buffer+i) = inw(0x1F0);
		}
		return true;
	}
	return false;
}

bool readSector(short sectors, long LBA, char* buffer)
{
	char status=-1;

	// Shall be set to one
	outb(0x1F6, 0x40);

	//outb(0x1F2, (sectors >> 8));
	//outb(0x1F3, (LBA >> 24) & 0xFF);
	//outb(0x1F4, 0x00);
	//outb(0x1F5, 0x00);

	// Number of sectors to read
	outb(0x1F2, (sectors & 0xFF));

	// LBA position of the data
	outb(0x1F3, (LBA & 0xFF));
	outb(0x1F4, (LBA >> 8 ) & 0xFF);
	outb(0x1F5, (LBA >> 16) & 0xFF);

	// READ SECTOR(S) EXT - 24h
	outb(0x1F7, 0x24);
	status = inb(0x1F7);

	// Poll the status while it's BSY, or till DRQ or ERR is set
	while ((status & 0x80) != 0 || (status & 0x09) == 0)
	{	status = inb(0x1F7);
	}

	// If ERR is not set, read the data
	if((status & 0x01) == 0)
	{	for(long i=0; i<512; i+=2)
		{	*(short*)(buffer+i) = inw(0x1F0);
		}
		return true;
	}
	return false;
}

void main()
{
	idt_init();
	clearScreen();

	char buffer[256];
	getLine(buffer, 256, '\n');
	putStr(buffer);
	putStr("\n");
}
