#include <kernel/asm.h>
#include <drivers/keyboard.h>
#include <drivers/screen.h>
#include <drivers/iobuffers.h>

#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60

char lshift;     // Pressed state of the left shift key
char rshift;     // Pressed state of the right shift key
char capsLock;   // On/Off state of the caps lock

// PS/2 Keyboard character set for normal key presses
const char normCharset[] = {
	 0 ,  0 , '1', '2', '3', '4', '5', '6', '7', '8', 
	'9', '0', '-', '=',  0 ,  0 , 'q', 'w', 'e', 'r',
	't', 'y', 'u', 'i', 'o', 'p', '[', ']',  0 ,  0 ,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0 ,'\\', 'z', 'x', 'c', 'v', 'b', 'n',
	'm', ',', '.', '/',  0 , '*',  0 , ' ',  0 ,  0 ,
	 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	 0 , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
};

// PS/2 Keyboard character set for shift key presses
const char shiftCharset[] = {
	 0 ,  0 , '!', '@', '#', '$', '%', '^', '&', '*', 
	'(', ')', '_', '+',  0 ,  0 , 'Q', 'W', 'E', 'R',
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',  0 ,  0 ,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
	'\"', '~', 0 , '|', 'Z', 'X', 'C', 'V', 'B', 'N',
	'M', '<', '>', '?',  0 , '*',  0 , ' ',  0 ,  0 ,
	 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	 0 , '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.',  0 ,  0 ,  0 ,  0 ,  0 ,  0
};

void keyboardHandler(void) 
{	
	unsigned char status;
	unsigned char keyCode;
	unsigned char character;

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
		else if((keyCode & 0x80) == 0)
		{	putChar(character);
			tempBuffer[tempIndex] = character;
			tempIndex++;
		}
	}	
}

// Returns a keyboard character based on the key code
// Takes into consideration the shift key and caps lock
char keyChar(char code)
{
	return (lshift || rshift || capsLock) ? shiftCharset[code] : normCharset[code];
}

// Handles system key presses with functionalities 
void sysKey(char code, char state)
{
	switch(code)
	{
		case KEY_LSHIFT:   lshift = (state == KEY_DOWN); break;
		case KEY_RSHIFT:   rshift = (state == KEY_DOWN); break;
		case KEY_CAPSLOCK: 
			if(state == KEY_DOWN)
			{	capsLock = !capsLock;
			}
			break;
		case KEY_BACKSPACE:
			if(state == KEY_DOWN)
			{	if(tempIndex > 0)
				{	tempIndex--;
					tempBuffer[tempIndex] = 0;
					delChar();
				}
			}
			break;
		case KEY_ENTER:
			if(state == KEY_DOWN)
			{	for(int srci=0, dsti=inputIndex; srci<tempIndex; srci++, dsti++)
				{	inputBuffer[dsti] = tempBuffer[srci];
				}

				inputIndex += tempIndex;
				inputBuffer[inputIndex++]='\n';

				tempIndex=0;
				nextLine();
			}
			break;
	}
}