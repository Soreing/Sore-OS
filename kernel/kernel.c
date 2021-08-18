#include <kernel/asm.h>
#include <kernel/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>
#include <drivers/filesys.h>

void main()
{
	idt_init();
	loadFileSystem();
	clearScreen();
}
