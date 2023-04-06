// #define BIN_TYPE BIN_CPM <---- DEFINED IN MAKEFILE
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "NabuTracker.h"
#include "nabu-games-patterns.h"
#include "tetris-nt.h"
#include "tetris.h"

uint8_t tb[64];
uint8_t tb8[4];
uint8_t tb16[6];

void initDisplay() {
    initNABULib();
    nt_init(music);
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false, false); //uint8_t bgColor, bool bigSprites, bool scaleSprites, bool autoScroll, don't split thirds
    vdp_enableVDPReadyInt();
    vdp_loadPatternTable(FAT,0x330);
    //Load same colour into every colour table cell.
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    vdp_setPatternColor(0x41);
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
    //Tetris block colours
    vdp_colorizePattern(0x0c, VDP_CYAN, VDP_BLACK);
    vdp_colorizePattern(0x11, VDP_DARK_BLUE, VDP_BLACK);
    vdp_colorizePattern(0x12, VDP_LIGHT_RED, VDP_BLACK);
    vdp_colorizePattern(0x13, VDP_LIGHT_YELLOW, VDP_BLACK);
    vdp_colorizePattern(0x14, VDP_LIGHT_GREEN, VDP_BLACK);
    vdp_colorizePattern(0x15, VDP_DARK_RED, VDP_BLACK);
    vdp_colorizePattern(0x16, VDP_MAGENTA, VDP_BLACK);
    
    vdp_setBackDropColor(VDP_DARK_YELLOW);          //Set border color
}

void setupMap() {
    //plot the map graphics
    uint8_t col = 7;
    uint8_t line = 0;
    for (int i = 0; i<480; i++) {

        // IO_VDPDATA = MAP[i];
        vdp_setCharAtLocationBuf(col, line, MAP[i]);

        col = col + 1;
        if (col > 26) {
            col = 7;
            line = line + 1;
        }
    }
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();

    //write the static words
    vdp_setCursor2(19,1);
    vdp_print("NEXT");
    vdp_setCursor2(19,8);
    vdp_print("SCORE");
    vdp_setCursor2(19,12);
    vdp_print("H.SCORE");
    vdp_setCursor2(19,16);
    vdp_print("LINES");
    vdp_setCursor2(19,20);
    vdp_print("LEVEL");
}

void displayTet(uint8_t x, uint8_t y, uint8_t tet_index, uint8_t frame) {
    uint8_t idx = tets[tet_index].first + frame;
    for(uint8_t i = 0; i < 4; i ++) {
        for (uint8_t j = 0; j < 4; j ++) {
            if (TETRONIMOS[idx][i*4+j] != 0x20) {   //don't print spaces as they might delete existing stuff.
                vdp_setCharAtLocationBuf(x+j, y+i, TETRONIMOS[idx][i*4+j]);
            }
        }
    }
}

void clearTet(uint8_t x, uint8_t y, uint8_t tet_index, uint8_t frame) {
    uint8_t idx = tets[tet_index].first + frame;
    for(uint8_t i = 0; i < 4; i ++) {
        for (uint8_t j = 0; j < 4; j ++) {  // only delete when the block part is not a space.
            if (TETRONIMOS[idx][i*4+j] != 0x20) {
                vdp_setCharAtLocationBuf(x+j, y+i, 0x20);
            }
        }
    }
}

bool isSpaceFree(uint8_t x, uint8_t y, uint8_t tet_index, uint8_t frame) {
    uint8_t idx = tets[tet_index].first + frame;
    for(uint8_t i = 0; i < 4; i ++) {
        for (uint8_t j = 0; j < 4; j ++) {  // only check when block part is not a space.
            if (TETRONIMOS[idx][i*4+j] != 0x20) {
                if (vdp_getCharAtLocationBuf(x+j,y+i) != 0x20) {
                    return false;
                }
            }
        }
    }
    // if we get here, then there is space.
    return true;
}

void clearPlayArea() {
    for( uint8_t i = 1; i<23; i++) {
        for( uint8_t j = 8; j < 18; j ++ ) {
            vdp_setCharAtLocationBuf(j, i, 0x20);
        }
    }
}

void clearLinesAndDropDown(uint8_t line_num) {
    for( uint8_t j = 8; j < 18; j ++ ) {
        vdp_setCharAtLocationBuf(j, line_num, 0x20);
    }

    for ( uint8_t i = line_num; i > 1; i-- ) {
        for( uint8_t j = 8; j < 18; j ++ ) {
            vdp_setCharAtLocationBuf(j, i, vdp_getCharAtLocationBuf(j,i-1));
        }
    }
    for( uint8_t j = 8; j < 18; j ++ ) {
        vdp_setCharAtLocationBuf(1,j,0x20);
    }
}

uint8_t checkCompletedLines(uint8_t *lines) {
    uint8_t completed_lines = 0;
    bool complete;
    for( uint8_t i = 1; i<23; i++) {
        complete = true;
        for( uint8_t j = 8; j < 18; j ++ ) {
            if (vdp_getCharAtLocationBuf(j, i) == 0x20) {
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

uint16_t getHighScore() {
    uint16_t hs;
    #if BIN_TYPE == BIN_CPM
        FILE * fp = fopen("tetris.dat", "r");
        if (fp) {
            fscanf(fp, "%d", &hs);
            fclose(fp);
        } else {
            hs = 0;
        }
    #else
        hs = 0;
    #endif
    return hs;
}

void setHighScore(uint16_t hs) {
    #if BIN_TYPE == BIN_CPM
        FILE * fp = fopen("tetris.dat", "w");
        fprintf(fp, "%d", hs);
        fclose(fp);
    #else
        (void)hs;
    #endif
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
    uint16_t high_score = getHighScore();
    uint8_t level = 1;
    uint8_t cleared_lines = 0;
    uint8_t hard_drop_flag = 0;

    // Keep track of button and down so we can action down on release event
    // and not allow multiple rotate actions without a release between.
    uint8_t btn_state = 0;
    uint8_t dn_state = 0;

    sprintf(tb8, "%d", cleared_lines);
    vdp_setCursor2(20,18);
    vdp_print(tb8);

    sprintf(tb16, "%d", score);
    vdp_setCursor2(20,10);
    vdp_print(tb16);

    sprintf(tb16, "%d", high_score);
    vdp_setCursor2(20,14);
    vdp_print(tb16);

    sprintf(tb8, "%d", level);
    vdp_setCursor2(20,21);
    vdp_print(tb8);

    while (running) {
        n = new_block();
        f = 0;              // first frame.
        uint8_t x = 10;     // middle of play area
        uint8_t y = 1;      // top row of play area

        displayTet(21, 3, n, 0);
        displayTet(x,y,t,f);

        while (true) {
            vdp_waitVDPReadyInt();
            vdp_refreshViewPort();
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
                    hard_drop_flag = 1; // add fallen lines to score during hard drop.
                    score = score + 1;  // the guidelines say, number of lines passed + 1
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
                    clearTet(x,y,t,f);
                    if(isSpaceFree(x-1,y,t,f)) {
                        x --;
                        displayTet(x,y,t,f);
                    } else {
                        displayTet(x,y,t,f);
                    }
                } else if (key == '.') {
                    clearTet(x,y,t,f);
                    if(isSpaceFree(x+1,y,t,f)) {
                        x ++;
                        displayTet(x,y,t,f);
                    } else {
                        displayTet(x,y,t,f);
                    }
                } else if (key == 'z') {
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
                    hard_drop_flag = 1; // guidelines state that score increases for each line passed in a hard drop
                    score = score + 1;  // guidelines state that score += 1 when hard dropping.
                } else if (key == 0x1b) {
                    running = false;
                    break;
                }
            }

            if (ticks % game_speed == 0) {  // game speed modifier
                clearTet(x,y,t,f);
                if (isSpaceFree(x, y + 1, t, f)) {
                    y++;
                    if(hard_drop_flag) {
                        score = score + 1;
                    }
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
            if (completed_lines == 1) {
                score = score + 40;
            } else if (completed_lines == 2) {
                score = score + 100;
            } else if (completed_lines == 3) {
                score = score + 300;
            } else if (completed_lines == 4) {
                score = score + 1200;
            }

            if(high_score < score) {
                high_score = score;
            }

            sprintf(tb16, "%d", score);
            vdp_setCursor2(20,10);
            vdp_print(tb16);

            sprintf(tb16, "%d", high_score);
            vdp_setCursor2(20,14);
            vdp_print(tb16);

            sprintf(tb8, "%d", cleared_lines);
            vdp_setCursor2(20,18);
            vdp_print(tb8);

            sprintf(tb8, "%d", level);
            vdp_setCursor2(20,21);
            vdp_print(tb8);
        }
        clearTet(21, 3, n, 0);  //clear the next block ready for the new one.
        t = n;
        game_speed = 30-(3*level);
        hard_drop_flag = 0;          //always reset hard drop flag
        if (game_speed < 1)
            game_speed = 1;
    }

    // Running is false - lets pause a bit so the player can see their screen.
    // Show the final score.
    if(high_score < score) {
        high_score = score;
    }
    sprintf(tb16, "%d", score);
    vdp_setCursor2(20,10);
    vdp_print(tb16);
    sprintf(tb16, "%d", high_score);
    vdp_setCursor2(20,14);
    vdp_print(tb16);
    nt_stopSounds();
    uint16_t timer = 0;
    while (timer < 180) { //3 seconds
        vdp_waitVDPReadyInt();
        timer ++;
    }
    setHighScore(high_score);
}

bool menu() {
    clearPlayArea();
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();

    vdp_setCursor2(13-(9/2),3);
    vdp_print("PRESS KEY");
    vdp_setCursor2(13-(8/2),4);
    vdp_print("TO START");
    vdp_setCursor2(13-(10/2),7);
    vdp_print(",(<)  LEFT");
    vdp_setCursor2(13-(10/2),8);
    vdp_print(".(>) RIGHT");
    vdp_setCursor2(13-(9/2),9);
    vdp_print("X & Z ROT");
    vdp_setCursor2(13-(10/2),10);
    vdp_print("SPACE DROP");
    vdp_setCursor2(13-(8/2) ,12);
    vdp_print("JOYSTICK");
    vdp_setCursor2(13-(10/2) ,13);
    vdp_print("BTN ROTATE");
    vdp_setCursor2(13-(9/2) ,14);
    vdp_print("DOWN DROP");
    vdp_setCursor2(13-(8/2),17);
    vdp_print("ESC QUIT");
    vdp_setCursor2(13-(10/2),19);
    vdp_print("PRODUCTION");
    vdp_setCursor2(13-(4/2),20);
    vdp_print("DAVE");
    vdp_setCursor2(13-(4/2),21);
    vdp_print("V3.1");

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
    }
    vdp_disableVDPReadyInt();
    #if BIN_TYPE == BIN_HOMEBREW
    __asm
        di
        rst 0x00
    __endasm;
    #endif
}