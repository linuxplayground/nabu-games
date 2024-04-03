/*
 * GUI written by productiondave <github.com/linuxplayground>
 * Uses NABU-LIB by DJ-Sures and some example code he gave me.
 */

#define DISABLE_CURSOR
#define byte uint8_t

#include "NABU-LIB.h"
#include "RetroNET-IAControl.h"

#include "imagui.h"

byte i = 0;
byte state = 1;
byte current_menu_id = 0;
byte namebuf[64];
byte authorbuf[16];
byte descrbuf[256];
byte newsbuf[512];
byte newsdate[20];
byte parent_id = 0;
byte channel_id = 0;
byte prev_channel_id = 0;
byte channel_length = 0;

/* Set the color of all patterns */
void vdp_setPatternColor(uint8_t color) __z88dk_fastcall {
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i < 0x1800; i++) {
        IO_VDPDATA = color;
        nop();
        nop();
        nop();
        nop();
    }
}

/* sets color of main menu text */
void color_main_menu() {
    for (i=0x0d; i<0x1a; i++) {
        vdp_colorizePattern(i, VDP_WHITE, VDP_BLACK);
    }
}

/* Prints into the frame buffer starting at x,y.  Truncates at end of line
 * preserving the border.
 */
void printAtLocationBuf(uint8_t x, uint8_t y, uint8_t *text) {
    uint16_t offset = y * _vdpCursorMaxXFull + x;
    uint8_t *start = text;
    while (*start != 0x00) {
        if (x< 31) {
            _vdp_textBuffer[offset] = *start;
            offset++;
            start++;
            x++;
        } else {
            break;
        }
    }
}

/* Prints into the buffer starting at x,y.  Wraps at the boarder on space.*/
void printWrappedAtLocationBuf(byte x, byte y, byte width, byte *text) {
    byte *tb = _vdp_textBuffer + y * 32 + x;
    byte lineLength = 0;

    while (*text != 0x00) {
        if (*text == '\n') {
            tb = _vdp_textBuffer + (++y * 32) + x;
            lineLength = 0;
            text++;
        } else {
            if (*text == ' ') {
                text ++;
                byte *startOfNextWord = text;
                while (*startOfNextWord != ' ' &&
                       *startOfNextWord != 0x00)
                    startOfNextWord ++;
                byte nextWordLength = startOfNextWord - text;
                if (lineLength + nextWordLength > width) {
                    tb = _vdp_textBuffer + (++y * 32) + x;
                    lineLength = 0;
                } else {
                    *++tb = ' ';
                    lineLength ++;
                }
            } 
            *tb = *text;

            tb ++;
            text ++;
            lineLength ++;
        }
    }
}

/* Initialise NABU-LIB, load graphics and colors */
void init(void) {
    initNABULib();
    vdp_clearVRAM();

    vdp_initG2Mode(1, false, false, false, false);
    vdp_enableVDPReadyInt();

    vdp_loadPatternTable(ASCII,1024);

    vdp_setPatternColor(0x41);

    vdp_colorizePattern(0x01, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x02, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x03, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x04, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x05, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x06, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x07, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x08, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x09, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x0A, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x0B, VDP_DARK_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x0C, VDP_WHITE, VDP_LIGHT_BLUE);  // Fast forward
    vdp_colorizePattern(0x1A, VDP_DARK_RED, VDP_BLACK); // Double red line
    vdp_colorizePattern(0x1B, VDP_WHITE, VDP_LIGHT_BLUE); // Fast backward
    vdp_colorizePattern(0x1C, VDP_WHITE, VDP_LIGHT_BLUE); // Button

    color_main_menu();

    vdp_setBackDropColor(VDP_DARK_YELLOW);
}


/* Delay by 1/60ths of a second*/
void delay(byte frames) __z88dk_fastcall {
    while(frames > 0) {
        vdp_waitVDPReadyInt();
        frames --;
    }
}

/* Routine to send a hard reset to the Nabu so that it loads again from ROM.*/
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

/* Draws the MAP from imagui.h This is faster than a for loop */
void draw_screen(void) {
    uint16_t count = 0;
    byte *m = map;
    byte *b = _vdp_textBuffer;

    while(count < 768) {
        *b = *m;
        b++;
        m++;
        count++;
    }
}

/* invert the colors of whichever menu is currently selected.
 * Adds some additional delay between calls to colorizePattern as I have
 * observed some glitches when rapidly switching between selected menu items
 * SPELLING Notwithstanding.
 */
void hilight_main_menu(void) {
    color_main_menu();
    vdp_waitVDPReadyInt();  // This might be useful to ensure smooth color loading into vram.
    switch (current_menu_id) {
        case 0:
            for(i=0x0d;i<0x11;i++) {
                vdp_colorizePattern(i, VDP_BLACK, VDP_WHITE);
                nop();
                nop();
            }
            break;
        case 1:
            for(i=0x11;i<0x15;i++) {
                vdp_colorizePattern(i, VDP_BLACK, VDP_WHITE);
                nop();
                nop();
            }
            break;
        case 2:
            for(i=0x15;i<0x1a;i++) {
                vdp_colorizePattern(i, VDP_BLACK, VDP_WHITE);
                nop();
                nop();
            }
            break;
    }
}

/* Moves the cursor for channel navigation between items
 * SPELLING Notwithstanding
 */
void hilight_channel(void) {
    vdp_setCharAtLocationBuf(1,prev_channel_id+3,0x20);
    vdp_setCharAtLocationBuf(1,channel_id+3,0x0C);
    prev_channel_id = channel_id;
    // If we are in the parents then show the description below.
    if (state == 1) {
        ia_getParentDescription(channel_id, descrbuf);
        for(byte i=1;i<31;i++) {
            vdp_setCharAtLocationBuf(i,11,0x1A);
            vdp_setCharAtLocationBuf(i,21,0x1A);
        }
        for(byte i=12;i<21;i++){
            for (byte j=1; j<31; j++) {
                vdp_setCharAtLocationBuf(j,i,0x00);
            }
        }
        printWrappedAtLocationBuf(1,12,29,descrbuf);
    }
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();
}

/* Waits for Joystick and Keyboard input.  Accepts input from any of the 4
 * joystick ports.  Keyboard takes priority.
 */
byte wait_for_joy(void) {
    while(true) {
        if (isKeyPressed()) {
            switch(getChar()) {
                case 0xe0:
                    return Joy_Right;
                    break;
                case 0xe1:
                    return Joy_Left;
                    break;
                case 0xe2:
                    return Joy_Up;
                    break;
                case 0xe3:
                    return Joy_Down;
                    break;
                case 0x0d:
                case 0x0a:
                    return Joy_Button;
                    break;
#if BIN_TYPE == BIN_CPM     //Only escape back to CPM if in CPM.
                case 0x1b:
                    __asm
                        rst 0x00;
                    __endasm;
                    break;
#endif
            }
        }
        for (i=0; i<4; i++) {
            switch (_joyStatus[i] & (Joy_Down | Joy_Up | Joy_Left | Joy_Right | Joy_Button) ) {
                case Joy_Down:
                    while (_joyStatus[i] & Joy_Down);
                    return Joy_Down;
                    break;
                case Joy_Up:
                    while (_joyStatus[i] & Joy_Up);
                    return Joy_Up;
                    break;
                case Joy_Left:
                    while (_joyStatus[i] & Joy_Left);
                    return Joy_Left;
                    break;
                case Joy_Right:
                    while (_joyStatus[i] & Joy_Right);
                    return Joy_Right;
                    break;
                case Joy_Button:
                    while (_joyStatus[i] & Joy_Button);
                    return Joy_Button;
                    break;
            }
        }
    }
}

/* Clears the main panel window */
void clear_main_panel(void) {
    for(i=3; i<23; i++) {
        printAtLocationBuf(1,i,"                              ");
    }
}

/* Loads the parent channels into the HOME menu. */
void load_parent_channels(void) {
    clear_main_panel();
    channel_length = ia_getParentCount();
    for(i=0; i<channel_length; i++) {
        ia_getParentName(i,namebuf);
        printAtLocationBuf(2, 3+i, namebuf);
    }
    vdp_setCharAtLocationBuf(1,22,0x1C);
    printAtLocationBuf(2,22,"SELECT");
}

/* Loads the sub channels */
void load_channels(void) {
    clear_main_panel();
    channel_length = ia_getChildCount(parent_id);
    for(i=0; i<channel_length; i++) {
        ia_getChildName(parent_id, i,namebuf);
        printAtLocationBuf(2, 3+i, namebuf);
    }
    vdp_setCharAtLocationBuf(1,22,0x1B);
    printAtLocationBuf(2,22,"BACK");
    vdp_setCharAtLocationBuf(7,22,0x1C);
    printAtLocationBuf(8,22,"DETAILS AND LAUNCH");

}

/* Loads the news title, news date and news content and prints it to the main
 * window.  News content is wrapped
 */
void load_news(void) {
    clear_main_panel();
    ia_getNewsTitle(namebuf);
    ia_getNewsDate(newsdate);
    ia_getNewsContent(newsbuf);

    printAtLocationBuf(1,3,newsdate);
    printAtLocationBuf(3,4,namebuf);
    printWrappedAtLocationBuf(1,6,29,newsbuf);
}

/* Displays the content of the about string stored in imagui.h.  Content is
 * wrapped.
 */
void load_about() {
    clear_main_panel();
    printWrappedAtLocationBuf(1,3,30,about);
}

/* Loads the channel details screen when a user navigates to the right */
void load_channel_detail(void) {
    clear_main_panel();

    ia_getChildName(parent_id, channel_id, namebuf);
    ia_getChildAuthor(parent_id, channel_id, authorbuf);
    ia_getChildDescription(parent_id, channel_id, descrbuf);

    printAtLocationBuf(1,3,namebuf);
    printAtLocationBuf(1,4,"By:");
    printAtLocationBuf(4,4, authorbuf);
    for(byte i=1;i<31;i++) {
        vdp_setCharAtLocationBuf(i,5,0x1A);
        vdp_setCharAtLocationBuf(i,21,0x1A);
    }
    printWrappedAtLocationBuf(1,7,29,descrbuf);

    vdp_setCharAtLocationBuf(1,22,0x1B);
    printAtLocationBuf(2,22,"BACK");
    vdp_setCharAtLocationBuf(7,22,0x1C);
    printAtLocationBuf(8,22,"LAUNCH");
}


/* Displays a short message, pauses for 2 seconds and then launches the 
 * selected channel by issuing a hard reset
 */
void launch() {
    clear_main_panel();
    ia_getChildName(parent_id, channel_id, namebuf);
    printAtLocationBuf(1, 5, "Loading...");
    printAtLocationBuf(1, 6, namebuf);
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();

    ia_setSelection(parent_id, channel_id);
    delay(120);
    hard_reset();
}

/* Main program loop.  This loop is responsible for managing all the navigation
 * state transitions, and refreshing the framebuffer.
 */
void main(void) {
    init();
    vdp_clearScreen();
    draw_screen();

    current_menu_id = 0;
    hilight_main_menu();
    load_parent_channels();
    hilight_channel();

    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();

    byte joy;

    /*
    * Left and right always move the main menu
    * Down will take you to the channels
    * While in channels, left and right will move you straight 
    * news or about, Down again brings you back to channels.
    */
    while (true) {
        joy = wait_for_joy();
        switch (state) {
            case 0:         // Main menu
                if (joy == Joy_Left) {
                    current_menu_id --;
                    if (current_menu_id == 0xFF) {
                        current_menu_id = 2;
                    } else if (current_menu_id == 0x00) {
                        state = 1;
                        load_parent_channels();
                        parent_id = 0;
                        channel_id = 0;
                        hilight_main_menu();
                        hilight_channel();
                        break;
                    }
                } else if (joy == Joy_Right) {
                    current_menu_id ++;
                    if (current_menu_id > 2) {
                        current_menu_id = 0;
                        state = 1;
                        load_parent_channels();
                        parent_id = 0;
                        channel_id = 0;
                        hilight_main_menu();
                        hilight_channel();
                        break;
                    }
                } else if (joy == Joy_Down) {
                    state = 1;
                    load_parent_channels();
                    prev_channel_id = 0;
                    channel_id = 0;
                    hilight_channel();
                    break;
                }

                if (current_menu_id == 0) {
                    load_parent_channels();
                    prev_channel_id = 0;
                    channel_id = 0;
                    hilight_channel();
                    state = 1;
                } else if (current_menu_id == 1) {
                    load_news();
                } else if (current_menu_id == 2) {
                    load_about();
                }
                hilight_main_menu();
                break;
            case 1:         // Parent channels
                if (joy == Joy_Up) {
                    channel_id --;
                    if (channel_id == 0xFF) {
                        channel_id = channel_length-1;
                    }
                } else if (joy == Joy_Down) {
                    channel_id ++;
                    if (channel_id > channel_length-1) {
                        channel_id = 0;
                    }
                } else if (joy == Joy_Left) {
                    current_menu_id = 2;
                    state =0;
                    hilight_main_menu();
                    load_about();
                    break;
                } else if (joy == Joy_Right) {
                    current_menu_id = 1;
                    state = 0;
                    hilight_main_menu();
                    load_news();
                    break;
                } else if (joy == Joy_Button) {
                    parent_id = channel_id;
                    load_channels();
                    channel_id = 0;
                    state = 2;
                }
                hilight_channel();
                break;
            case 2:         // Sub channels
                if (joy == Joy_Left) {
                    state = 1;
                    load_parent_channels();
                    parent_id = 0;
                    channel_id = 0;
                    hilight_channel();
                    break;
                } else if (joy == Joy_Up) {
                    channel_id --;
                    if (channel_id == 0xFF) {
                        channel_id = channel_length-1;
                    }
                } else if (joy == Joy_Down) {
                    channel_id ++;
                    if (channel_id > channel_length-1) {
                        channel_id = 0;
                    }
                } else if (joy == Joy_Button) {
                    state = 3;
                    load_channel_detail();
                    break;
                }
                hilight_channel();
                break;
            case 3:
                if (joy == Joy_Left) {
                    state = 2;
                    load_channels();
                    hilight_channel();
                    break; 
                } else if (joy == Joy_Button) {
                    launch();
                }
                break;
        } 
        vdp_waitVDPReadyInt();
        vdp_refreshViewPort();
    }
}

