/*
 * GUI written by productiondave <github.com/linuxplayground>
 * Uses NABU-LIB by DJ-Sures and some example code he gave me.
 */

#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "RetroNET-IAControl.h"

#include "NabuTracker.h"
#include "nabu-games.h"
#include "imagui.h"

char namebuf[64];
char textbuf[32];


/*
 * Lifted from DJs code drop in discord.
 * This function, besides converting a string to upper will also truncate it
 * by inserting a 0x00 into position 29 if we get there.
 */
void toupper(uint8_t *text) {
    uint8_t p = 0;
    for (uint16_t i = 0; text[i] != 0x00; i++) {
        if (text[i] >= 'a')
            text[i] = text[i] - 32;
        p ++;
        if (p==29) {
            text[i] = 0x00;
            return;
        }
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
    vdp_loadPatternTable(FAT,768);
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
        printAtLocationBuf(menu_x, 0, arr[i]);
        menu_x = menu_x + str_len(arr[i]) + 1;
    }
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();
}


/*
 * Display parents screen
 */
uint8_t parents_screen(uint8_t p_count) {
    vdp_clearScreen();
    draw_menu(main_menu, MAIN_MENU_LEN);
    printAtLocationBuf(1, 2, "MAIN MENU");
    for (uint8_t i = 0; i< p_count; i++) {
        ia_getParentName(i, namebuf);
        toupper(namebuf);
        sprintf(textbuf, "%d %s", i, namebuf);
        printAtLocationBuf(1, 6+i, textbuf);
    }
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort(); 

    uint8_t p_id = 0xFF;
    while (p_id == 0xFF) {
        uint8_t _key = getChar();
        if ( _key == 0x1b) return 254;

        if ( _key >= 0x30 || _key < p_count + 0x30 ) {
            p_id = _key - 0x30;
        }
        vdp_waitVDPReadyInt();
    }
    return p_id;
}

/*
 * Display children screen
 */
uint8_t children_screen(uint8_t p_id) {
    vdp_clearScreen();
    draw_menu(main_menu, MAIN_MENU_LEN);

    ia_getParentName(p_id, namebuf);
    toupper(namebuf);

    printAtLocationBuf(1, 2, namebuf);

    uint8_t c_count = ia_getChildCount(p_id);

    for (uint8_t i = 0; i < c_count; i++) {
        ia_getChildName(p_id, i, namebuf);
        toupper(namebuf);
        sprintf(textbuf, "%d %s", i, namebuf);
        printAtLocationBuf(1, 6+i, textbuf);
    }

    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();

    uint8_t c_id = 255;
    while (c_id == 255) {
        uint8_t _key = getChar();
        if ( _key >= 0x30 || _key < c_count + 0x30 ) {
            c_id = _key - 0x30;
        }
        vdp_waitVDPReadyInt();
    }
    return c_id;
}

void main(void) {
    init();
    draw_menu(main_menu, 4);
    // Get channel data
    uint8_t p_count = ia_getParentCount();

    while (1) {
        uint8_t p_id = parents_screen(p_count);
        if (p_id == 254) break;
        uint8_t c_id = children_screen(p_id);

        ia_getChildName(p_id, c_id, namebuf);
        toupper(namebuf);
        sprintf(textbuf, "--> %s", namebuf);
        printAtLocationBuf(2, 23, textbuf);

        ia_setSelection(p_id, c_id);
        vdp_waitVDPReadyInt();
        vdp_refreshViewPort();
        delay(2000);
        //hardReset();
    }
}
