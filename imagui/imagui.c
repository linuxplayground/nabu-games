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
byte state = 0;
byte current_menu_id = 0;
byte namebuf[64] = {0};
byte authorbuf[16];
byte tmpbuf[512];
byte newsdate[20];
byte parent_id = 0;
byte channel_id = 0;
byte prev_channel_id = 0;
byte channel_length = 0;
byte news_length = 0;
byte news_id = 0;
byte page_num = 1;
byte num_pages = 1;

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

/* utillity function to get stirng length */
byte str_len(char *text) {
    byte l = 0;
    while (*text != 0x00) {
        l++;
        text++;
    }
    return l;
}
/* sets color of main menu text */
void color_main_menu() {
    for (i=0x0d; i<0x1a; i++) {
        vdp_colorizePattern(i, VDP_WHITE, VDP_BLACK);
        nop();
        nop();
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
    vdp_colorizePattern(0x0C, VDP_WHITE, VDP_BLACK);  // fast forward
    vdp_colorizePattern(0x1A, VDP_DARK_RED, VDP_BLACK); // Double red line
    vdp_colorizePattern(0x1B, VDP_WHITE, VDP_BLACK); // left
    vdp_colorizePattern(0x1C, VDP_WHITE, VDP_BLACK);
    vdp_colorizePattern(0x1D, VDP_WHITE, VDP_BLACK);
    vdp_colorizePattern(0x1E, VDP_WHITE, VDP_BLACK);
    vdp_colorizePattern(0x1F, VDP_WHITE, VDP_BLACK);

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
            for(i=0x15;i<0x1a;i++) {
                vdp_colorizePattern(i, VDP_BLACK, VDP_WHITE);
                nop();
                nop();
            }
            break;
        // case 2:
        //     for(i=0x15;i<0x1a;i++) {
        //         vdp_colorizePattern(i, VDP_BLACK, VDP_WHITE);
        //         nop();
        //         nop();
        //     }
        //     break;
    }
}

/* Clears the main panel window */
void clear_main_panel(void) {
    for(i=3; i<23; i++) {
        printAtLocationBuf(1,i,"                              ");
    }
}

void update_channels(void) {
    clear_main_panel();
    for(i=page_num * 16 - 16; i<page_num * 16; i++) {
        if (i<channel_length) {
            ia_getChildName(parent_id, i, namebuf);
            printAtLocationBuf(2, 3+(i%16), namebuf);
        }
    }
    vdp_setCharAtLocationBuf(1,22,0x1B);
    printAtLocationBuf(2,22,"Back");
    vdp_setCharAtLocationBuf(7,22,0x1C);
    printAtLocationBuf(8,22,"Select");
    vdp_setCharAtLocationBuf(15,22,0x1D);
    printAtLocationBuf(16,22,"Next");
    sprintf(namebuf, "%u/%u", page_num, num_pages + 1);
    printAtLocationBuf(31-3,22,namebuf);
}

/* Moves the cursor for channel navigation between items
 * SPELLING Notwithstanding
 */
void hilight_channel(void) {
    if (state == 2) {
        if (channel_id >= page_num * 16) {
            page_num ++;
            update_channels();
        }
        if (channel_id < page_num * 16 - 16) {
            page_num --;
            if (page_num == 0) page_num = 1;
            update_channels();
        }
    }
    vdp_setCharAtLocationBuf(1,(prev_channel_id%16)+3,0x20);
    vdp_setCharAtLocationBuf(1,(channel_id%16)+3,0x0C);
    prev_channel_id = channel_id;
    // If we are in the parents then show the description below.
    if (state == 1) {
        ia_getParentDescription(channel_id, tmpbuf);
        for(byte i=1;i<31;i++) {
            vdp_setCharAtLocationBuf(i,11,0x1A);
            vdp_setCharAtLocationBuf(i,21,0x1A);
        }
        for(byte i=12;i<21;i++){
            for (byte j=1; j<31; j++) {
                vdp_setCharAtLocationBuf(j,i,0x00);
            }
        }
        printWrappedAtLocationBuf(1,12,29,tmpbuf);
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

/* Loads the parent channels into the HOME menu. */
void load_parent_channels(void) {
    printAtLocationBuf(12,1,"                   ");
    ia_getCurrentDateTimeStr("dd/MM/yyyy HH:mm",17,namebuf);
    printAtLocationBuf(31-16,1,namebuf);
    clear_main_panel();
    channel_length = ia_getParentCount();
    for(i=0; i<channel_length; i++) {
        ia_getParentName(i,namebuf);
        printAtLocationBuf(2, 3+i, namebuf);
    }
    vdp_setCharAtLocationBuf(1,22,0x1C);
    printAtLocationBuf(2,22,"Select");
    channel_id = 0;
    hilight_channel();
}

/* Loads the sub channels */
void load_channels(void) {
    channel_length = ia_getChildCount(parent_id);
    num_pages = channel_length / 16;
    page_num = 1; 
    update_channels();
}

void draw_icon_box(void) {
    vdp_setCharAtLocationBuf(2,7,0x08);
    vdp_setCharAtLocationBuf(3,7,0x01);
    vdp_setCharAtLocationBuf(4,7,0x01);
    vdp_setCharAtLocationBuf(5,7,0x09);
    vdp_setCharAtLocationBuf(2,8,0x02);
    vdp_setCharAtLocationBuf(5,8,0x02);
    vdp_setCharAtLocationBuf(2,9,0x02);
    vdp_setCharAtLocationBuf(5,9,0x02);
    vdp_setCharAtLocationBuf(2,10,0x0B);
    vdp_setCharAtLocationBuf(3,10,0x01);
    vdp_setCharAtLocationBuf(4,10,0x01);
    vdp_setCharAtLocationBuf(5,10,0x0A);
}

/* Loads the news title, news date and news content and prints it to the main
 * window.  News content is wrapped
 */
void load_news(void) {
    clear_main_panel();
    for(byte i=1;i<31;i++) {
        vdp_setCharAtLocationBuf(i,5,0x1A);
        vdp_setCharAtLocationBuf(i,21,0x1A);
    }
    ia_extended_getNewsTitleById(news_id, namebuf);
    ia_extended_getNewsDateById(news_id, newsdate);
    ia_extended_getNewsContentById(news_id, tmpbuf);
    printAtLocationBuf(1,3,newsdate);
    printAtLocationBuf(3,4,namebuf);

    printWrappedAtLocationBuf(7,7,22,tmpbuf);

    draw_icon_box();

    ia_extended_getNewsIconTilePattern(news_id, tmpbuf);
    vdp_loadPatternToId(0xf0, tmpbuf);
    vdp_loadPatternToId(0xf1, tmpbuf + 8);
    vdp_loadPatternToId(0xf2, tmpbuf + 16);
    vdp_loadPatternToId(0xf3, tmpbuf + 24);

    ia_extended_getNewsIconTileColor(news_id, tmpbuf);
    vdp_loadColorToId(0xf0, tmpbuf);
    vdp_loadColorToId(0xf1, tmpbuf + 8);
    vdp_loadColorToId(0xf2, tmpbuf + 16);
    vdp_loadColorToId(0xf3, tmpbuf + 24);

    vdp_setCharAtLocationBuf(3, 8, 0xf0);
    vdp_setCharAtLocationBuf(3, 9, 0xf1);
    vdp_setCharAtLocationBuf(4, 8, 0xf2);
    vdp_setCharAtLocationBuf(4, 9, 0xf3);

    vdp_setCharAtLocationBuf(1,22,0x1C);
    printAtLocationBuf(2,22,"Channels");
    vdp_setCharAtLocationBuf(11,22,0x1E);
    vdp_setCharAtLocationBuf(12,22,0x1F);
    printAtLocationBuf(13,22,"Cycle");
    sprintf(namebuf, "%u/%u", news_id+1, news_length);
    printAtLocationBuf(31-str_len(namebuf),22,namebuf);
}


/* Displays the content of the about string stored in imagui.h.  Content is
 * wrapped.
 */
void load_about(void) {
    clear_main_panel();
    for(byte i=1;i<31;i++) {
        vdp_setCharAtLocationBuf(i,6,0x1A);
        vdp_setCharAtLocationBuf(i,21,0x1A);
    }
    printWrappedAtLocationBuf(1,3,30, about_title);
    printWrappedAtLocationBuf(1,8,30,about_descr);

    vdp_setCharAtLocationBuf(1,22,0x1B);
    printAtLocationBuf(2,22,"News");

}

/* Loads the channel details screen when a user navigates to the right */
void load_channel_detail(void) {
    clear_main_panel();

    ia_getChildName(parent_id, channel_id, namebuf);
    ia_getChildAuthor(parent_id, channel_id, authorbuf);
    ia_getChildDescription(parent_id, channel_id, tmpbuf);

    printAtLocationBuf(1,3,namebuf);
    printAtLocationBuf(1,4,"By:");
    printAtLocationBuf(4,4, authorbuf);
    for(byte i=1;i<31;i++) {
        vdp_setCharAtLocationBuf(i,5,0x1A);
        vdp_setCharAtLocationBuf(i,21,0x1A);
    }
    printWrappedAtLocationBuf(7,7,22,tmpbuf);

    draw_icon_box();

    ia_getChildIconTilePattern(parent_id, channel_id, tmpbuf);
    vdp_loadPatternToId(0xf0, tmpbuf);
    vdp_loadPatternToId(0xf1, tmpbuf + 8);
    vdp_loadPatternToId(0xf2, tmpbuf + 16);
    vdp_loadPatternToId(0xf3, tmpbuf + 24);

    ia_getChildIconTileColor(parent_id, channel_id, tmpbuf);
    vdp_loadColorToId(0xf0, tmpbuf);
    vdp_loadColorToId(0xf1, tmpbuf + 8);
    vdp_loadColorToId(0xf2, tmpbuf + 16);
    vdp_loadColorToId(0xf3, tmpbuf + 24);

    vdp_setCharAtLocationBuf(3, 8, 0xf0);
    vdp_setCharAtLocationBuf(3, 9, 0xf1);
    vdp_setCharAtLocationBuf(4, 8, 0xf2);
    vdp_setCharAtLocationBuf(4, 9, 0xf3);

    vdp_setCharAtLocationBuf(1,22,0x1B);
    printAtLocationBuf(2,22,"Back");
    vdp_setCharAtLocationBuf(7,22,0x1C);
    printAtLocationBuf(8,22,"Launch");
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

    news_length = ia_extended_getNewsCount();
    news_id = news_length - 1;
    load_news();

    ia_getCurrentDateTimeStr("dd/MM/yyyy HH:mm",17,namebuf);
    printAtLocationBuf(31-16,1,namebuf);

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
            case 0:
                if( joy == Joy_Left && current_menu_id == 1 ) {
                    current_menu_id = 0;
                    hilight_main_menu();
                    load_news();
                } else if ( joy == Joy_Right && current_menu_id == 0) {
                    current_menu_id = 1;
                    load_about();
                    hilight_main_menu();
                } else if (joy == Joy_Button) {
                    if( current_menu_id == 0) {
                            state = 1;
                            channel_id = 0;
                            color_main_menu();
                            load_parent_channels();
                    }
                } else if (joy == Joy_Up) {
                    news_id --;
                    if (news_id == 0xFF) news_id = news_length-1;
                    load_news();
                } else if (joy == Joy_Down) {
                    news_id ++;
                    if (news_id > news_length-1) news_id = 0;
                    load_news();
                }
                break;
            case 1:
                if( joy == Joy_Up ) {
                    channel_id--;
                    if (channel_id == 0xFF) {
                        state = 0;
                        current_menu_id = 0;
                        hilight_main_menu();
                        vdp_setCharAtLocationBuf(1,prev_channel_id+3,0x20);
                        load_news();
                        break;
                    } 
                } else if ( joy == Joy_Down ) {
                    channel_id++;
                    if (channel_id > channel_length-1) channel_id = 0;
                } else if ( joy == Joy_Button ) {
                    state = 2;
                    parent_id = channel_id;

                    printAtLocationBuf(12,1,"                   ");
                    ia_getParentName(parent_id, namebuf);
                    printAtLocationBuf(31-str_len(namebuf),1,namebuf);

                    channel_id = 0;
                    load_channels();
                    hilight_channel();
                    break;
                }
                hilight_channel();
                break;
            case 2:
                if (joy == Joy_Up) {
                    channel_id--;
                    if (channel_id == 0xFF) channel_id = channel_length - 1;
                } else if (joy == Joy_Down) {
                    channel_id++;
                    if (channel_id > channel_length -1) channel_id = 0;
                } else if (joy == Joy_Right) {
                    channel_id = (page_num + 1) * 16 - 16;
                    if (channel_id > channel_length) channel_id = 0;
                } else if (joy == Joy_Left) {
                    state = 1;
                    channel_id = 0;
                    color_main_menu();
                    load_parent_channels();
                    break;
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
        } 
        vdp_waitVDPReadyInt();
        vdp_refreshViewPort();
    }
}

