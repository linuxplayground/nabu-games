#define BIN_TYPE BIN_CPM
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "../NABULIB/NABU-LIB.h"
#include "../NABULIB/NabuTracker.h"
#include "../include/patterns.h"
#include "tetris-nt.h"
#include "tetris.h"
#include <arch/z80.h> // for z80_delay_ms()

uint8_t tb[64];

void initDisplay() {
    initNABULib();
    nt_init(music);
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false); //uint8_t bgColor, bool bigSprites, bool scaleSprites, bool autoScroll
    vdp_enableVDPReadyInt();
    vdp_loadPatternTable(FAT,0x330);
    //Load same colour into every colour table cell.
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i<_vdpColorTableSize; i++) {
        IO_VDPDATA = 0x41;                  // Dark blue on black
    }
    //Make UPPER CASE ASCII in LIGHT GREEN
    for (uint8_t i = 65; i < 92; i++) {
        vdp_colorizePattern(i,VDP_LIGHT_GREEN, VDP_BLACK);
    }
    //Make numbers in LIGHT RED
    for (uint8_t i = 48; i < 58; i++) {
        vdp_colorizePattern(i, VDP_DARK_RED, VDP_BLACK);
    }
    //Make puctuation in LIGHT GREEN
    for (uint8_t i = 32; i<=47; i++) {
        vdp_colorizePattern(i,VDP_LIGHT_GREEN, VDP_BLACK);
    }
    for (uint8_t i = 58; i<=64; i++) {
        vdp_colorizePattern(i,VDP_LIGHT_GREEN, VDP_BLACK);
    }
    for (uint8_t i = 91; i<=96; i++) {
        vdp_colorizePattern(i,VDP_LIGHT_GREEN, VDP_BLACK);
    }
    for (uint8_t i = 123; i<126; i++) {
        vdp_colorizePattern(i,VDP_LIGHT_GREEN, VDP_BLACK);
    }
    //Tetris blocks are MAGENTA
    vdp_colorizePattern(0x0c, VDP_MAGENTA, VDP_BLACK);

    vdp_setBackDropColor(VDP_DARK_YELLOW);          //Set border color
}

void setupMap() {
    //plot the map graphics
    uint8_t col = 6;
    uint8_t line = 0;
    vdp_setWriteAddress(_vdpPatternNameTableAddr + (line * 32) + 6);
    for (int i = 0; i<504; i++) {

        IO_VDPDATA = MAP[i];

        col = col + 1;
        if (col > 26) {
            col = 6;
            line = line + 1;
            vdp_setCursor2(col,line);
            vdp_setWriteAddress(_vdpPatternNameTableAddr + (line * 32) + 6);
        }
    }
    //write the static words
    vdp_setCursor2(20,1);
    vdp_print("NEXT");
    vdp_setCursor2(20,9);
    vdp_print("LINES");
    vdp_setCursor2(20,14);
    vdp_print("SCORE");
    vdp_setCursor2(20,19);
    vdp_print("LEVEL");

}

void displayTet(uint8_t x, uint8_t y, uint8_t tet_index, uint8_t frame) {
    uint8_t idx = tets[tet_index].first + frame;
    for(uint8_t i = 0; i < 4; i ++) {
        for (uint8_t j = 0; j < 4; j ++) {
            if (TETRONIMOS[idx][i*4+j] != 0x20) {   //don't print spaces as they might delete existing stuff.
                vdp_writeCharAtLocation(x+j, y+i, TETRONIMOS[idx][i*4+j]);
            }
        }
    }
}

void clearTet(uint8_t x, uint8_t y, uint8_t tet_index, uint8_t frame) {
    uint8_t idx = tets[tet_index].first + frame;
    for(uint8_t i = 0; i < 4; i ++) {
        for (uint8_t j = 0; j < 4; j ++) {  // only delete when the block part is not a space.
            if (TETRONIMOS[idx][i*4+j] != 0x20) {
                vdp_writeCharAtLocation(x+j, y+i, 0x20);
            }
        }
    }
}

bool isSpaceFree(uint8_t x, uint8_t y, uint8_t tet_index, uint8_t frame) {
    uint8_t idx = tets[tet_index].first + frame;
    for(uint8_t i = 0; i < 4; i ++) {
        for (uint8_t j = 0; j < 4; j ++) {  // only check when block part is not a space.
            if (TETRONIMOS[idx][i*4+j] != 0x20) {
                if (vdp_getCharAtLocationVRAM(x+j,y+i) != 0x20) {
                    return false;
                }
            }
        }
    }
    // if we get here, then there is space.
    return true;
}

void clearLinesAndDropDown(uint8_t line_num) {
    for( uint8_t j = 7; j < 19; j ++ ) {
        vdp_writeCharAtLocation(j, line_num, 0x20);
    }

    for ( uint8_t i = line_num; i > 1; i-- ) {
        for( uint8_t j = 7; j < 19; j ++ ) {
            vdp_writeCharAtLocation(j, i, vdp_getCharAtLocationVRAM(j,i-1));
        }
    }
    for( uint8_t j = 7; j < 19; j ++ ) {
        vdp_writeCharAtLocation(1,j,0x20);
    }
}

uint8_t checkCompletedLines(uint8_t *lines) {
    uint8_t completed_lines = 0;
    bool complete;
    for( uint8_t i = 1; i<23; i++) {
        complete = true;
        for( uint8_t j = 7; j < 19; j ++ ) {
            if (vdp_getCharAtLocationVRAM(j, i) == 0x20) {
                complete = false;
                break;
            }
        }
        if (complete) {
            lines[completed_lines] = i;     // store the completed line number in the lines array.
            completed_lines ++;
            playNoteDelay(2,67,25);
            if( completed_lines == 4 ) {      // We can't clear more than 4 lines at once.
                break;
            }
        }
    }
    return completed_lines;
}

int new_block() {
    //Reset used blocks if we hav
    if (block_count == 7) {
        for(uint8_t i=0; i<7; i++) {
            used_blocks[i] = 0;
        }
        block_count = 0;
    }
    uint8_t t;
    while(true) {
        t = rand()%7;   // choose first block
        if (!used_blocks[t])
            break;
    }
    block_count ++;
    used_blocks[t] = 1;
    return t;
}

void play() {
    uint8_t t = new_block();
    uint8_t n;
    int8_t f;
    bool running = true;
    uint16_t ticks = 0;
    uint16_t game_speed = 30;
    uint8_t lines[4];
    uint16_t score = 0;
    uint8_t level = 1;
    uint8_t cleared_lines = 0;

    // Keep track of button and down so we can action down on release event
    // and not allow multiple rotate actions without a release between.
    uint8_t btn_state = 0;
    uint8_t dn_state = 0;



    vdp_setCursor2(21,11);
    vdp_writeUInt8(cleared_lines);
    vdp_setCursor2(21,16);
    vdp_writeInt16(score);
    vdp_setCursor2(21,21);
    vdp_writeInt8(level);

    while (running) {
        n = new_block();
        f = 0;              // first frame.
        uint8_t x = 11;     // middle of play area
        uint8_t y = 1;      // top row of play area

        displayTet(21, 3, n, 0);
        displayTet(x,y,t,f);

        while (true) {
            vdp_waitVDPReadyInt();
            ticks ++;
            if (ticks % 7 == 0) {
                nt_handleNote();
            }
            if (ticks % 6 == 0) {  //only take joystick input every 6th tick (every 100ms)
                if (getJoyStatus(0) & Joy_Left) {
                    clearTet(x,y,t,f);
                    if(isSpaceFree(x-1,y,t,f)) {
                        x --;
                        displayTet(x,y,t,f);
                    } else {
                        displayTet(x,y,t,f);
                    }
                }
                if (getJoyStatus(0) & Joy_Right) {
                    clearTet(x,y,t,f);
                    if(isSpaceFree(x+1,y,t,f)) {
                        x ++;
                        displayTet(x,y,t,f);
                    } else {
                        displayTet(x,y,t,f);
                    }
                }
                if (getJoyStatus(0) & Joy_Down && dn_state == 0) {
                    playNoteDelay(2,24,15);
                    game_speed = 1;     // cant be 0 because of modulus of game ticks.
                    dn_state = 1;
                }

                if (getJoyStatus(0) & Joy_Button && btn_state == 0) {
                    clearTet(x,y,t,f);
                    f ++;
                    if (f > tets[t].count-1) {
                        f = 0;
                    }

                    if (isSpaceFree(x,y,t,f)) {
                        displayTet(x,y,t,f);
                    } else {
                        f --;
                        if (f < 0) {
                            f = tets[t].count-1;
                        }
                        displayTet(x,y,t,f);
                    }
                    btn_state = 1;
                }
            } else {
                if (!(getJoyStatus(0) & Joy_Button)) {  // we try to know i the joystick is in nutral.
                    btn_state = 0;
                }
                if (!(getJoyStatus(0) & Joy_Down)) {
                    dn_state = 0;
                }
            }

            if (isKeyPressed()) {
                uint8_t key = getChar();
                if (key == ',') {
                    //playNoteDelay(2,36,3);
                    clearTet(x,y,t,f);
                    if(isSpaceFree(x-1,y,t,f)) {
                        x --;
                        displayTet(x,y,t,f);
                    } else {
                        displayTet(x,y,t,f);
                    }
                } else if (key == '.') {
                    //playNoteDelay(2,36,3);
                    clearTet(x,y,t,f);
                    if(isSpaceFree(x+1,y,t,f)) {
                        x ++;
                        displayTet(x,y,t,f);
                    } else {
                        displayTet(x,y,t,f);
                    }
                } else if (key == 'z') {
                    //playNoteDelay(2,38,3);
                    clearTet(x,y,t,f);
                    f --;
                    if (f < 0) {
                           f = tets[t].count-1;
                    }
                    if (isSpaceFree(x,y,t,f)) {
                        displayTet(x,y,t,f);
                    } else {
                        f ++;
                        if (f > tets[t].count-1) {
                            f = 0;
                        }
                        displayTet(x,y,t,f);
                    }
                } else if (key == 'x') {
                    //playNoteDelay(2,38,3);
                    clearTet(x,y,t,f);
                    f ++;
                    if (f > tets[t].count-1) {
                        f = 0;
                    }

                    if (isSpaceFree(x,y,t,f)) {
                        displayTet(x,y,t,f);
                    } else {
                        f --;
                        if (f < 0) {
                            f = tets[t].count-1;
                        }
                        displayTet(x,y,t,f);
                    }
                } else if (key == ' ') {
                    playNoteDelay(2,24,15);
                    game_speed = 1;     // cant be 0 because of modulus of game ticks.
                } else if (key == 0x1b) {
                    running = false;
                    break;
                }
            }

            if (ticks % game_speed == 0) {  // game speed modifier
                // play a note when falling
                // if (game_speed > 1)
                //     playNoteDelay(0,9,3);   // don't play if have done a drop.
                clearTet(x,y,t,f);
                if (isSpaceFree(x, y + 1, t, f)) {
                    y++;
                    displayTet(x, y, t, f);
                } else {
                    displayTet(x, y, t, f);
                    if(y==1) {
                        running = false;
                    }
                    break;
                }
            }
        }
        // Check for compelted lines
        uint8_t completed_lines = checkCompletedLines(lines);

        if(completed_lines > 0) {
            for(uint8_t i=0; i<completed_lines; i++) {
                clearLinesAndDropDown(lines[i]);
                cleared_lines ++;
                if (cleared_lines == 10) {
                    level = level + 1;
                    cleared_lines = 0;
                }
            }
            score = score + completed_lines;
            if (completed_lines == 4) {
                score = score + 10;
            }
            vdp_setCursor2(21,16);
            vdp_writeInt16(score);
            vdp_setCursor2(21,11);
            vdp_writeUInt8(cleared_lines);
            vdp_setCursor2(21,21);
            vdp_writeInt8(level);
        }
        clearTet(21, 3, n, 0);  //clear the next block ready for the new one.
        t = n;
        game_speed = 30-(3*level);
        if (game_speed < 1)
            game_speed = 1;
    }

}

bool menu() {
    vdp_setCursor2(13-(9/2),3);
    vdp_print("PRESS ANY");
    vdp_setCursor2(13-(12/2),4);
    vdp_print("KEY TO START");
    vdp_setCursor2(13-(10/2),7);
    vdp_print(",(<)  LEFT");
    vdp_setCursor2(13-(10/2),8);
    vdp_print(".(>) RIGHT");
    vdp_setCursor2(13-(12/2),9);
    vdp_print("X & Z ROTATE");
    vdp_setCursor2(13-(10/2),10);
    vdp_print("SPACE DROP");
    vdp_setCursor2(13-(8/2) ,12);
    vdp_print("JOYSTICK");
    vdp_setCursor2(13-(12/2),13);
    vdp_print("LEFT & RIGHT");
    vdp_setCursor2(13-(10/2) ,14);
    vdp_print("BTN ROTATE");
    vdp_setCursor2(13-(12/2) ,15);
    vdp_print("DOWN TO DROP");
    vdp_setCursor2(13-(8/2),17);
    vdp_print("ESC QUIT");

    while (true) {
        if (getJoyStatus(0) & Joy_Button) {
            return true;
        }
        if (isKeyPressed()) {
            if (getChar() == 0x1b) {
                return false;
            } else {
                return true;
            }
        }
    }
}

void main() {
    initDisplay();
    setupMap();
    while(menu()) {
        setupMap();
        play();
        nt_stopSounds();
    }
    vdp_disableVDPReadyInt();
}