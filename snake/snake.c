#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "NabuTracker.h"
#include "nabu-games-patterns.h"
#include "snake.h"

// These patterns have been redefined in patterns.h
#define HEAD_UP    0x01
#define HEAD_DOWN  0x02
#define HEAD_LEFT  0x03
#define HEAD_RIGHT 0x04
#define APPLE      0x05


void init() {
    initNABULib();
    initNABULIBAudio();
    nt_init(music);
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false, false);
    vdp_loadPatternTable(FAT,0x330);
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    vdp_setPatternColor(0x41);

    vdp_setBackDropColor(VDP_DARK_YELLOW);                  //Set border color

    // overwrite special colours.
    vdp_colorizePattern(APPLE,      VDP_LIGHT_GREEN, VDP_BLACK);   //Apple
    vdp_colorizePattern(HEAD_UP,    VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_DOWN,  VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_LEFT,  VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_RIGHT, VDP_MAGENTA,     VDP_BLACK);       //Snake head
}

bool menu() {
    char _score_str[10];
    sprintf(_score_str, "SCORE: %03d", score);
    vdp_clearScreen();
    vdp_setCursor2(16-(12/2),4);
    vdp_print("SNAKE - V3.0");
    vdp_setCursor2(16-(17/2),5);
    vdp_print("BY PRODUCTIONDAVE");
    vdp_setCursor2(16-(13/2),8);
    vdp_print("JOYSTICK ONLY");
    vdp_setCursor2(16-(17/2), 11);
    vdp_print("BTN TO PLAY AGAIN");
    vdp_setCursor2(16-(10/2),13);
    vdp_print(_score_str);
    vdp_setCursor2(16-(11/2), 16);
    vdp_print("ESC TO QUIT");
    while(true) {
        if (getJoyStatus(0) & Joy_Button)
            return true;
        if (isKeyPressed()) {
            uint8_t _key = getChar();
            if(_key == 0x1b)
                return false;
            if(_key == ' ')
                return true;
        }
    }
}

void new_apple() {
    bool _taken = true;
    while(_taken) {
        uint8_t x = rand() % 32;
        uint8_t y = rand() % 24;
        if (vdp_getCharAtLocationBuf(x,y) == 0x00) {
            _taken = false;
            apple.x = x;
            apple.y = y;
        }      
    }
    vdp_setCharAtLocationBuf(apple.x, apple.y, applechar);
}
void line(int x0, int y0, int x1, int y1) {

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) >> 1;

    while (x0 != x1 || y0 != y1) {
        vdp_setCharAtLocationBuf(x0, y0, 0x15);
        int e2 = err;
        if (e2 > -dx) {
            err -= dy; 
            x0 += sx; 
        }
        if (e2 < dy) {
            err += dx; 
            y0 += sy; 
        }
    }
}

void drawLevel(uint8_t *level, uint8_t len) {
    uint8_t *start = level;
    uint8_t *end = start + len;
    uint8_t lines = 0;
    do {
        uint8_t a0 = *start;
        start ++;
        uint8_t b0 = *start;
        start ++;
        uint8_t a1 = *start;
        start ++;
        uint8_t b1 = *start;
        start ++;
        line(a0,b0,a1,b1);
        lines ++;
    } while (start != end);
}

void clearTextBuffer() {
    for(uint16_t i=0; i<0x300; i++) {
        _vdp_textBuffer[i] = 0x00;
    }
}

void setup_level() {
    clearTextBuffer();
    //Reset snake length and circ buffer
    head.x = 15;
    head.y = 20;
    head.dir = HEAD_RIGHT;
    head.pattern = HEAD_RIGHT;
    buffer_head = -2;
    buffer_tail = -2;
    ticks = 0;
    segments = more_segments;
    if (level > 0) {
        uint8_t levelsize = *levels[level-1];
        drawLevel(levels[level-1] + 1, levelsize);
    }
    new_apple();
    pause=true;
}

void setup_game() {
    clearTextBuffer();
    initNABULIBAudio(); // reset to regular music mode
    score = 0;
    level = 0;
    game_speed = 10;
    crashed = false;
    setup_level();
}

void delay(uint8_t frames) {
    uint8_t ticks = 0;
    while(true) {
        vdp_waitVDPReadyInt();
        ticks ++;
        if (ticks > frames)
            break;
    }
}

void game() {

    while(!crashed) {
        if (pause) {
            nt_stopSounds();
            bool flash = false;
            delay(30);
            while(true) {
                if (getJoyStatus(0) & Joy_Button) {
                    break;
                }
                delay(10);
                flash = ~flash;
                if(flash) {
                    vdp_setCharAtLocationBuf(head.x, head.y, head.pattern);
                } else {
                    vdp_setCharAtLocationBuf(head.x, head.y, 0x00);
                }
                vdp_waitVDPReadyInt();
                vdp_refreshViewPort();
            }
            vdp_setCharAtLocationBuf(head.x, head.y, 0x00);
            pause = false;
        }
        //get input - Joystick move in 4 directions. (EASY MODE)
        if(getJoyStatus(0) & Joy_Left && head.dir != HEAD_RIGHT) {
            head.dir = HEAD_LEFT;
            head.pattern = HEAD_LEFT;
        }
        else if(getJoyStatus(0) & Joy_Up && head.dir != HEAD_DOWN) {
            head.dir = HEAD_UP;
            head.pattern = HEAD_UP;
        }
        else if(getJoyStatus(0) & Joy_Right && head.dir != HEAD_LEFT) {
            head.dir = HEAD_RIGHT;
            head.pattern = HEAD_RIGHT;
        }
        else if(getJoyStatus(0) & Joy_Down && head.dir != HEAD_UP) {
            head.dir = HEAD_DOWN;
            head.pattern = HEAD_DOWN;
        }

        if (ticks % game_speed == 0) {
            // move the head
            if (head.dir == HEAD_LEFT) {
                head.x --;
            }
            else if (head.dir == HEAD_UP) {
                head.y --;
            }
            else if (head.dir == HEAD_RIGHT) {
                head.x ++;
            }
            else if (head.dir == HEAD_DOWN) {
                head.y ++;
            }

            //check for collisions

            //Border Collisions first
            if(head.x < 0 || head.x > 31 || head.y < 0 || head.y > 23)
                crashed = true;

            uint8_t _next = vdp_getCharAtLocationBuf(head.x, head.y);
            if(_next == APPLE) {
            //Apple check
                segments = more_segments;
                new_apple();
                score ++;
                if (score % 5 == 0 && game_speed > 5) {
                    game_speed --;
                }
                if (score % 10 == 0) {
                    level ++;
                    if (level > 3)
                        level = 0;
                    setup_level();
                }
                playNoteDelay(1, 43, 15);
                playNoteDelay(2, 43, 15);
            } else if (_next != 0x00) {
                crashed = true;
            }

            // update buffer pointers
            if (buffer_head < BUFFSIZE) {
                buffer_head = buffer_head + 2;
            } else {
                buffer_head = 0;
            }
            circularBuffer[buffer_head] = head.x;
            circularBuffer[buffer_head + 1] = head.y;

            if(segments == 0) {
                if(buffer_tail < BUFFSIZE) {
                    buffer_tail = buffer_tail + 2;
                } else {
                    buffer_tail = 0;
                }
            } else {
                segments --;
            }
            //draw the head
            vdp_setCharAtLocationBuf(head.x, head.y, head.pattern);
            //clear the tail
            vdp_setCharAtLocationBuf(circularBuffer[buffer_tail], circularBuffer[buffer_tail + 1], 0x00);
            nt_handleNote();
        }
        if (!crashed) {
            vdp_waitVDPReadyInt();
            vdp_refreshViewPort();
            ticks ++;
        }
    }

    // play crash sound
    ayWrite(6,  0x0f);
    ayWrite(7,  0b11000111);
    ayWrite(8,  0x1f);
    ayWrite(9,  0x1f);
    ayWrite(10, 0x1f);
    ayWrite(11, 0xa0);
    ayWrite(12, 0x40);
    ayWrite(13, 0x00);
    // we have crashed - lets pause for a bit
    uint8_t timer = 0;
    while (timer < 180) { //3 seconds
        vdp_waitVDPReadyInt();
        timer ++;
    }
}

void main() {
    init();
    vdp_enableVDPReadyInt();
    while(menu()) {
        setup_game();
        game();
    };
    vdp_disableVDPReadyInt();
    #if BIN_TYPE == BIN_HOMEBREW
    __asm
        di
        rst 0x00
    __endasm;
    #endif
}