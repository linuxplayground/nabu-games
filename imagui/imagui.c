/*
 * GUI written by productiondave <github.com/linuxplayground>
 * Uses NABU-LIB by DJ-Sures and some example code he gave me.
 */

#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "RetroNET-IAControl.h"

//#include "NabuTracker.h"
#include "imagui.h"

char namebuf[64];
char textbuf[32];

void vdp_setPatternColor(uint8_t color) {
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i < 0x1800; i++) {
        IO_VDPDATA = color;
        nop();
        nop();
        nop();
        nop();
    }
}

void printAtLocationBuf(uint8_t x, uint8_t y, uint8_t *text) {
    uint16_t offset = y * _vdpCursorMaxXFull + x;
    uint8_t *start = text;

    while (*start != 0x00) {

      _vdp_textBuffer[offset] = *start;
      offset++;
      start++;
    }
}


void delay(uint8_t frames) {
    while(frames > 0) {
        vdp_waitVDPReadyInt();
        frames --;
    }
}

void hardReset(void) {
    __asm
        // Disable interrupts
        // di
        ld a,0xf3;
        ld (0x8880),a;

        // Enable rom shadow
        // lda a,0x02
        ld a,0x3e;
        ld (0x8881),a;
        ld a,0x02;
        ld (0x8882),a;

        // out(0x00),a
        ld a,0xd3;
        ld (0x8883),a;
        ld a,0x00;
        ld (0x8884),a;

        // restart at 0x00
        // rst 0x00
        ld a,0xc7;
        ld (0x8885),a;

        jp 0x8880;
    __endasm;
}

/*
 * Lifted from DJs code drop in discord.
 * This function, besides converting a string to upper will also truncate it
 * by inserting a 0x00 into position 29 if we get there.
 */
void toupper(uint8_t *text) {
    for (uint16_t i = 0; text[i] != 0x00; i++) {
        if (text[i] >= 'a')
            text[i] = text[i] - 32;
    }
}

/* needed because our strings in the arrays are pointers */
uint8_t str_len(char *text) {
    uint8_t len = 0;
    for (uint16_t i = 0; text[i] != 0x00; i++)
        len ++; 
    return len;
}

void init(void) {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false, false);
    vdp_loadPatternTable(ASCII,1024);
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    vdp_setPatternColor(0x41);
    vdp_setBackDropColor(VDP_DARK_YELLOW);                         //Set border color

    //initNABULIBAudio();
    vdp_enableVDPReadyInt();
}

/*
 * Draws the menu bar at the top of the screen.
 */
void draw_menu(int **arr, uint8_t size) {
    uint8_t menu_x = 0;

    for (uint8_t i = 0; i<size; i++) {
        printAtLocationBuf(menu_x + 1, 0, arr[i]);
        menu_x = menu_x + str_len(arr[i]) + 1;
    }
}

void underline_menu_item( uint8_t item_num, int**arr) {
    uint8_t menu_x = 1;
    uint8_t menu_y = 1;

    for (uint8_t i = 0; i<item_num; i++) {
        menu_x = menu_x + str_len(arr[i]) + 1;
    }
    for (uint8_t i = 0; i<str_len(arr[item_num]); i++) {
        printAtLocationBuf(menu_x + i, menu_y, "~");
    }
}

void main(void) {
    init();
    draw_menu(main_menu, MAIN_MENU_LEN);
    underline_menu_item(2, main_menu);

    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();

    while (true) {
        if (isKeyPressed()) {
            if (getChar() == 0x1b) {
                break;
            }
        }
    }
}
