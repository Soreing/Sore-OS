#include <kernel/asm.h>
#include <drivers/screen.h>

#define VIDEO_ADDRESS 0xB8000
#define WHITE_ON_BLACK 0x0F

int col = 0;
int row = 0;

// Updates the cater's position
void updateCursor()
{
	int pos = row * 80 + col/2;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (int) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (int) ((pos >> 8) & 0xFF));
}

// Clears the screen
void clearScreen()
{
    col = 0;
    row = 0;
    updateCursor();

    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
    for(int i=0; i<80*25*2; i+=2)
    {   vidmem[i] = 0; 
    }
}

// Puts a character on the screen
void putChar(char ch)
{
    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
    int offset = row*160 + col;

	vidmem[offset]   = ch;
	vidmem[offset+1] = WHITE_ON_BLACK;

    col+=2;
    if(col == 160)
    {   col = 0;
        row++;
    }

    updateCursor();
}

// Deletes a character from the screen
void delChar()
{
    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
    int offset = row*160 + col -2;

    if(offset >= 0)
    {   vidmem[offset]   = 0;
        vidmem[offset+1] = WHITE_ON_BLACK;

        col-=2;
        if(col < 0)
        {   col = 158;
            row--;
        }

        updateCursor();
    }
}