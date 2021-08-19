#ifndef SCREEN_H
#define SCREEN_H

#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_LIGHT_BROWN 14
#define COLOR_WHITE 15

// Sets foreground font folor of the text
void setFontColor(unsigned char color);

// Sets background font folor of the text
void setBackgrounColor(unsigned char color);

// Clears the screen
void clearScreen();

// Puts the caret on the next line on the screen
void nextLine();

// Puts a character on the screen
void putChar(char ch);

// Deletes a character from the screen
void delChar();

#endif