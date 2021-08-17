#include <kernel/asm.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>


#define PIC1_COMMAND	0x20
#define PIC1_DATA		0x21
#define PIC2_COMMAND	0xA0
#define PIC2_DATA		0xA1

#define ICW1_ICW4		0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE		0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL		0x08		/* Level triggered (edge) mode */
#define ICW1_INIT		0x10		/* Initialization - required! */
 
#define ICW4_8086		0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO		0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM		0x10		/* Special fully nested (not) */

#define EOI				0x20		/* End of Interrupt */

#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60

#define false 0
#define true  1

typedef long bool;

struct IDT_entry{
	unsigned short offset_lowerbits;
	unsigned short selector;
	unsigned char  zero;
	unsigned char  type_attr;
	unsigned short offset_higherbits;
};
 
struct IDT_entry IDT[256];

void idt_init(void) 
{
	extern int load_idt();
	extern int irq0();
	extern int irq1();
	extern int irq2();
	extern int irq3();
	extern int irq4();
	extern int irq5();
	extern int irq6();
	extern int irq7();
	extern int irq8();
	extern int irq9();
	extern int irq10();
	extern int irq11();
	extern int irq12();
	extern int irq13();
	extern int irq14();
	extern int irq15();

	unsigned long irq0_address;
	unsigned long irq1_address;
	unsigned long irq2_address;
	unsigned long irq3_address;          
	unsigned long irq4_address; 
	unsigned long irq5_address;
	unsigned long irq6_address;
	unsigned long irq7_address;
	unsigned long irq8_address;
	unsigned long irq9_address;          
	unsigned long irq10_address;
	unsigned long irq11_address;
	unsigned long irq12_address;
	unsigned long irq13_address;
	unsigned long irq14_address;          
	unsigned long irq15_address;         
	unsigned long idt_address;
	unsigned long idt_ptr[2];
 
	/* remapping the PIC */
	outb(PIC1_COMMAND, 	ICW1_INIT | ICW1_ICW4);
	outb(PIC2_COMMAND, 	ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, 	0x20);
	outb(PIC2_DATA, 	0x28);
	outb(PIC1_DATA, 	0x04);
	outb(PIC2_DATA, 	0x02);
	outb(PIC1_DATA, 	ICW4_8086);
	outb(PIC2_DATA, 	ICW4_8086);
	outb(PIC1_DATA,		0x0);
	outb(PIC2_DATA, 	0x0);
 
	irq0_address = (unsigned long)irq0; 
	IDT[32].offset_lowerbits = irq0_address & 0xffff;
	IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[32].zero = 0;
	IDT[32].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[32].offset_higherbits = (irq0_address & 0xffff0000) >> 16;
 
	irq1_address = (unsigned long)irq1; 
	IDT[33].offset_lowerbits = irq1_address & 0xffff;
	IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[33].zero = 0;
	IDT[33].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;
 
	irq2_address = (unsigned long)irq2; 
	IDT[34].offset_lowerbits = irq2_address & 0xffff;
	IDT[34].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[34].zero = 0;
	IDT[34].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[34].offset_higherbits = (irq2_address & 0xffff0000) >> 16;
 
	irq3_address = (unsigned long)irq3; 
	IDT[35].offset_lowerbits = irq3_address & 0xffff;
	IDT[35].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[35].zero = 0;
	IDT[35].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[35].offset_higherbits = (irq3_address & 0xffff0000) >> 16;
 
	irq4_address = (unsigned long)irq4; 
	IDT[36].offset_lowerbits = irq4_address & 0xffff;
	IDT[36].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[36].zero = 0;
	IDT[36].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[36].offset_higherbits = (irq4_address & 0xffff0000) >> 16;
 
	irq5_address = (unsigned long)irq5; 
	IDT[37].offset_lowerbits = irq5_address & 0xffff;
	IDT[37].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[37].zero = 0;
	IDT[37].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[37].offset_higherbits = (irq5_address & 0xffff0000) >> 16;
 
	irq6_address = (unsigned long)irq6; 
	IDT[38].offset_lowerbits = irq6_address & 0xffff;
	IDT[38].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[38].zero = 0;
	IDT[38].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[38].offset_higherbits = (irq6_address & 0xffff0000) >> 16;
 
	irq7_address = (unsigned long)irq7; 
	IDT[39].offset_lowerbits = irq7_address & 0xffff;
	IDT[39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[39].zero = 0;
	IDT[39].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[39].offset_higherbits = (irq7_address & 0xffff0000) >> 16;
 
	irq8_address = (unsigned long)irq8; 
	IDT[40].offset_lowerbits = irq8_address & 0xffff;
	IDT[40].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[40].zero = 0;
	IDT[40].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[40].offset_higherbits = (irq8_address & 0xffff0000) >> 16;
 
	irq9_address = (unsigned long)irq9; 
	IDT[41].offset_lowerbits = irq9_address & 0xffff;
	IDT[41].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[41].zero = 0;
	IDT[41].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[41].offset_higherbits = (irq9_address & 0xffff0000) >> 16;
 
	irq10_address = (unsigned long)irq10; 
	IDT[42].offset_lowerbits = irq10_address & 0xffff;
	IDT[42].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[42].zero = 0;
	IDT[42].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[42].offset_higherbits = (irq10_address & 0xffff0000) >> 16;
 
	irq11_address = (unsigned long)irq11; 
	IDT[43].offset_lowerbits = irq11_address & 0xffff;
	IDT[43].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[43].zero = 0;
	IDT[43].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[43].offset_higherbits = (irq11_address & 0xffff0000) >> 16;
 
	irq12_address = (unsigned long)irq12; 
	IDT[44].offset_lowerbits = irq12_address & 0xffff;
	IDT[44].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[44].zero = 0;
	IDT[44].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[44].offset_higherbits = (irq12_address & 0xffff0000) >> 16;
 
	irq13_address = (unsigned long)irq13; 
	IDT[45].offset_lowerbits = irq13_address & 0xffff;
	IDT[45].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[45].zero = 0;
	IDT[45].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[45].offset_higherbits = (irq13_address & 0xffff0000) >> 16;
 
	irq14_address = (unsigned long)irq14; 
	IDT[46].offset_lowerbits = irq14_address & 0xffff;
	IDT[46].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[46].zero = 0;
	IDT[46].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[46].offset_higherbits = (irq14_address & 0xffff0000) >> 16;
 
    irq15_address = (unsigned long)irq15; 
	IDT[47].offset_lowerbits = irq15_address & 0xffff;
	IDT[47].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[47].zero = 0;
	IDT[47].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[47].offset_higherbits = (irq15_address & 0xffff0000) >> 16;
 
	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;
 
	load_idt(idt_ptr);
 
}

void irq0_handler(void) 
{	outb(PIC1_COMMAND, EOI);
}
 
 // Keyboard irq handler
void irq1_handler(void) 
{	
	unsigned char status;

	char keyCode;
	char character;

	outb(PIC1_COMMAND, EOI);
	status = inb(KEYBOARD_STATUS_PORT);

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) 
	{	// Retrieve the key code from the keyboard data port
		keyCode = inb(KEYBOARD_DATA_PORT);

		// Handle the key press as a system key or a printable character
		character = keyChar(keyCode & 0x7F);
		if(character == 0)
		{	sysKey(keyCode & 0x7F, keyCode & 0x80);
		}
		else if(keyCode > 0)
		{	putChar(character);
			tempBuffer[tempIndex] = character;
			tempIndex++;
		}
	}	
}
 
void irq2_handler(void) 
{	outb(PIC1_COMMAND, EOI);
}
 
void irq3_handler(void) 
{	outb(PIC1_COMMAND, EOI);
}
 
void irq4_handler(void) 
{	outb(PIC1_COMMAND, EOI);
}
 
void irq5_handler(void) 
{	outb(PIC1_COMMAND, EOI);
}
 
void irq6_handler(void) 
{	outb(PIC1_COMMAND, EOI);
}
 
void irq7_handler(void) 
{	outb(PIC1_COMMAND, EOI);
}
 
void irq8_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}
 
void irq9_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}
 
void irq10_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}
 
void irq11_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}
 
void irq12_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}
 
void irq13_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}
 
void irq14_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}
 
void irq15_handler(void)
{	outb(PIC2_COMMAND, EOI);
	outb(PIC1_COMMAND, EOI);       
}

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
