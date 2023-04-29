#ifndef NABUGAMESLIB
#define NABUGAMESLIB

#include <strings.h>

/* Set up Graphics mode 2 and nabu games yellow border, blue text */
void init();
/* Print text into the frame buffer at location x, y */
void printAtLocationBuf(uint8_t x, uint8_t y, uint8_t *text);

/* Print text in the middle of the screen on row y*/
void centerText(char *text, uint8_t y);

/* Delay for count of frames */
void delay(uint8_t frames);

#include "nabu-games.c"
#endif