#ifndef NABUGAMESLIB
#define NABUGAMESLIB

#include <strings.h>
#include "nabu-games-patterns.h"

/* Extra function needed but not found in upstream nabu-lib*/
void vdp_setPatternColor(uint8_t color);

/* Print text into the frame buffer at location x, y */
void printAtLocationBuf(uint8_t x, uint8_t y, uint8_t *text);

/* Print text in the middle of the screen on row y*/
void centerText(char *text, uint8_t y);

/* Delay for count of frames */
void delay(uint8_t frames);

// Hack to push raw assembly into RAM (away from rom) so that
// we can re-enable shadow rom and do a reboot.  This will
// cause the NABU to reload it's rom from the internet adapter
// and boot whatever the internet adapter sends it.
// Next time the IA needs to send data, it will send the default
// app which is probably going to be the menu gui.
void hardReset(void);

#include "nabu-games.c"
#endif
