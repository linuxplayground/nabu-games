/*
 * GUI written by productiondave <github.com/linuxplayground>
 * Uses NABU-LIB by DJ-Sures and some example code he gave me.
 */

#define DISABLE_CURSOR
#define byte uint8_t

#include "NABU-LIB.h"
#include "RetroNET-IAControl.h"

//#include "NabuTracker.h"
#include "imagui.h"

byte main_menu_selected;

uint16_t i;

void vdp_setPatternColor(uint8_t color) __z88dk_fastcall {
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i < 0x1800; i++) {
        IO_VDPDATA = color;
    }
}

void g2_printColorChar(byte c, byte x, byte y, byte fg, byte bg) {
    //Find the pattern location on the screen.
    vdp_setWriteAddress(_vdpPatternGeneratorTableAddr + (y*256) + (x*8) );
    //write the 8 bytes of data into the pattern table.
    for (byte i=0; i<8; i++) {
        IO_VDPDATA = ASCII[i + (c*8)];
    }
    vdp_setWriteAddress(_vdpColorTableAddr + (y*256) + (x*8) );

    uint8_t color = fg<<4 | bg;
    for(byte i=0; i<8; i++) {
        IO_VDPDATA = color;
    }
}

void g2_printColor(char *text, byte x, byte y, byte fg, byte bg) {
    while (*text != 0) {
        g2_printColorChar(*text, x, y, fg, bg);
        x=x+1;
        if(x>31) {
            x=0;
            y=y+1;
        }
        text ++;
    }
}

void g2_clear_screen(void) {
    vdp_disableVDPReadyInt();
    // init all patterns to 0x00.  Pixels off.
    vdp_setWriteAddress(_vdpPatternGeneratorTableAddr);

    for(i = 0; i < 6144; i++)
        IO_VDPDATA = 0x00;

    vdp_setWriteAddress(_vdpColorTableAddr);
    for (i = 0; i < 6144; i++)
        IO_VDPDATA = 0xE1;
    vdp_enableVDPReadyInt();
}

void init(void) {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false, true);


    //Load a unique pattern name into each of the 3 segments of the name table
    vdp_setWriteAddress(_vdpPatternNameTableAddr);
    for (i=0; i<0x400; i++)
        IO_VDPDATA = i % 0x100;

    g2_clear_screen();

    vdp_setBackDropColor(VDP_DARK_YELLOW);

    vdp_enableVDPReadyInt();
}


void delay(byte frames) __z88dk_fastcall {
    while(frames > 0) {
        vdp_waitVDPReadyInt();
        frames --;
    }
}

void hard_reset(void) {
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
void toupper(byte *text) __z88dk_fastcall {
    for (uint16_t i = 0; text[i] != 0x00; i++) {
        if (text[i] >= 'a')
            text[i] = text[i] - 32;
    }
}

/* used when drawing the main menu */
byte str_len(char *text) {
    byte len = 0;
    for (byte i = 0; text[i] != 0x00; i++)
        len ++; 
    return len;
}

void draw_screen(void) {
    for (byte i=1; i<31; i++) {
        g2_printColorChar(0x01, i, 0, VDP_DARK_GREEN, VDP_BLACK);
        g2_printColorChar(0x01, i, 2, VDP_DARK_GREEN, VDP_BLACK);
        g2_printColorChar(0x01, i, 23, VDP_DARK_GREEN, VDP_BLACK);
    }

    for (byte i=1; i<23; i++) {
        g2_printColorChar(0x02, 0,i, VDP_DARK_GREEN, VDP_BLACK);
        g2_printColorChar(0x02, 31,i, VDP_DARK_GREEN, VDP_BLACK);
    }

    g2_printColorChar(0x08,0,0,VDP_DARK_GREEN,VDP_BLACK);
    g2_printColorChar(0x09,31,0,VDP_DARK_GREEN,VDP_BLACK);
    g2_printColorChar(0x0A,31,23,VDP_DARK_GREEN,VDP_BLACK);
    g2_printColorChar(0x0B,0,23,VDP_DARK_GREEN,VDP_BLACK);
    g2_printColorChar(0x07,0,2,VDP_DARK_GREEN,VDP_BLACK);
    g2_printColorChar(0x06,31,2,VDP_DARK_GREEN,VDP_BLACK);
}

void draw_main_menu(void) {
    byte x = 1;
    for(byte i=0; i<MAIN_MENU_LEN; i++) {
        g2_printColor(main_menu[i], x, 1, VDP_LIGHT_BLUE, VDP_BLACK);
        x = x + str_len(main_menu[i]) + 1;
    }
}

void select_main_menu() {
    draw_main_menu();
    byte x = 1;
    for (byte i=0; i<main_menu_selected; i++) {
        x = x + str_len(main_menu[i]) + 1;
    }
    g2_printColor(main_menu[main_menu_selected], x, 1, VDP_DARK_GREEN, VDP_BLACK);
}


void main(void) {
    init();
    draw_screen();
    draw_main_menu();
    main_menu_selected = 0;
    select_main_menu();

    while (!isKeyPressed());
}
