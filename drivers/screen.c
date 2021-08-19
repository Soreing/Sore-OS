#include <kernel/asm.h>
#include <drivers/screen.h>
#include <string.h>

#define VIDEO_ADDRESS  0xB8000
#define BACKUP_ADDRESS 0xB8FA0
#define WHITE_ON_BLACK 0x0F

unsigned char style = WHITE_ON_BLACK;

int column   = 0;   // Current column offset
int dispRow  = 0;   // Display row offset on the 1st page
int globRow  = 0;   // Global row offset in the backup pages
int lineShow = 0;   // Counter of which line is shown on the display page

// Sets foreground font folor of the text
void setFontColor(unsigned char color)
{   style = (style & 0xF0) | (color & 0x0F);
}

// Sets background font folor of the text
void setBackgrounColor(unsigned char color)
{   style = (style & 0x0F) | (color & 0xF0);
}

// Updates the cater's position
void updateCursor()
{
	int pos = dispRow * 80 + column/2;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (int) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (int) ((pos >> 8) & 0xFF));
}

// Resets the row and column counters to 0 
// Clears the display page and the backup page
void clearScreen()
{
    column   = 0;
    dispRow  = 0;
    globRow  = 0;
    lineShow = 0;
    updateCursor();

    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
    for(int i=0; i<8*80*25*2; i+=2)
    {   vidmem[i]   = 0;
        vidmem[i+1] = style; 
    }
}

// Scrolls the contents of the display page and starts a new line
// If the last backup page is full, it deletes it and shifts the other pages
void scrollLineDown()
{
    // Move the cursor back to 0 offset and down a line
    column = 0;
    globRow++;

    // Erase a page if the backup is full
    if(globRow == 175)
    {   int src = BACKUP_ADDRESS + 4000;
        int dst = BACKUP_ADDRESS;
        memcpy((void*)dst, (void*)src, 18000);
        globRow  -= 25;
        lineShow -= 25;
    }

    if(dispRow == 24)
    {   // Shift up all the lines
        memcpy((void*)VIDEO_ADDRESS, (void*)(VIDEO_ADDRESS+160), 24*160);
        lineShow++;

        // Erase the last line of display
        int lineStart = VIDEO_ADDRESS + 24*160;
        for(int i=0; i<160; i+=2)
        {   *(unsigned char*)(lineStart+i) = 0;
            *(unsigned char*)(lineStart+i+1) = style;
        }
    }
    else
    {   dispRow++;
    }
}

// Scrolls the contents of the display page and goes backa line
void scrollLineUp()
{
    // If there's more data in the backup pages
    // Paste the last 25 lines from the backup
    if(dispRow == 24 && globRow >= 25)
    {   int src = BACKUP_ADDRESS + (globRow-25) * 160;
        int dst = VIDEO_ADDRESS;
        memcpy((void*)dst, (void*)src, 25*160);
        lineShow--;
    }
    else
    {   dispRow--;
    }

    // Move the cursor back to the end and up a line
    column = 158;
    globRow--;
}

// Scrolls 25 lines up in the history and displays it
void scrollPageUp()
{
    int line = (lineShow-25) < 0 ? 0 : (lineShow-25);
    lineShow = line;

    int src = BACKUP_ADDRESS + line * 160;
    int dst = VIDEO_ADDRESS;
    memcpy((void*)dst, (void*)src, 25*160);
}

// Scrolls 25 lines down in the history and displays it
void scrollPageDown()
{
    int line = (lineShow+25) > (globRow-24) ? (globRow-24) : (lineShow+25);
    lineShow = line;

    int src = BACKUP_ADDRESS + line * 160;
    int dst = VIDEO_ADDRESS;
    memcpy((void*)dst, (void*)src, 25*160);
}

//Scrolls the page back to where the user was typing
void scrollPageBack()
{
    int line = (globRow-24) < 0 ? 0 : (globRow-24);
    lineShow = line;

    int src = BACKUP_ADDRESS + line * 160;
    int dst = VIDEO_ADDRESS;
    memcpy((void*)dst, (void*)src, 25*160);
}

// Puts a character on the screen
void putChar(char ch)
{
    // Get the resources for the display video memory and the backup pages
    unsigned char *dispmem  = (unsigned char*) VIDEO_ADDRESS;
    unsigned char *backmem = (unsigned char*) BACKUP_ADDRESS;
    int dispOffset = dispRow*160 + column;
    int globOffset = globRow*160 + column;

    // Scroll the page back to where the user was typing
    if(lineShow != globRow-24)
    {   scrollPageBack();
    }

    if(ch == '\n')
    {   scrollLineDown();
        return;
    }

    // Add the character to the display and backup memory
	dispmem[dispOffset] = backmem[globOffset] = ch;
	dispmem[dispOffset+1] = backmem[globOffset+1] = style;

    // Move the cursor, if at the end of the line, scroll down 
    column += 2;
    if(column == 160)
    {   scrollLineDown();
    }

    updateCursor();
}

// Deletes a character from the screen
void delChar()
{
    // Get the resources for the display video memory and the backup pages
    unsigned char *dispmem  = (unsigned char*) VIDEO_ADDRESS;
    unsigned char *backmem = (unsigned char*) BACKUP_ADDRESS;
    int dispOffset = dispRow * 160 + column -2;
    int globOffset = globRow * 160 + column -2;

    // Scroll the page back to where the user was typing
    if(lineShow != globRow-24)
    {   scrollPageBack();
    }

    if(dispOffset >= 0)
    {
        // Removes the character to the display and backup memory
        dispmem[dispOffset] = backmem[globOffset] = 0;
	    dispmem[dispOffset+1] = backmem[globOffset+1] = style;

        // Move the cursor, if at the beginning of the line, scroll up 
        column -= 2;
        if(column < 0)
        {   scrollLineUp();
        }

        updateCursor();
    }
}