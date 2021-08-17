#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEY_DOWN 0x00
#define KEY_UP 0x80

#define KEY_ESCAPE 0x01
#define KEY_BACKSPACE 0x0E
#define KEY_ENTER 0x1C
#define KEY_LCONTROL 0x1D
#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_LALT 0x38
#define KEY_CAPSLOCK 0x3A

// Returns a keyboard character based on the key code
// Takes into consideration the shift key and caps lock
char keyChar(char code);

// Handles system key presses with functionalities 
void sysKey(char code, char state);

#endif