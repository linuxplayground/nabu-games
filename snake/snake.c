#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "NabuTracker.h"
#include "nabu-games-patterns.h"
#include "snake.h"


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
    for (uint16_t i = 0; i<_vdpColorTableSize; i++) {
        IO_VDPDATA = 0x41;                  // Dark blue on black
        nop();                              // Chuck in some delay here for retro compatibility
        nop();
        nop();
        nop();
    }

    vdp_setBackDropColor(VDP_DARK_YELLOW);                  //Set border color

    // overwrite special colours.
    vdp_colorizePattern(APPLE,      VDP_LIGHT_GREEN, VDP_BLACK);   //Apple
    vdp_colorizePattern(HEAD_UP,    VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_DOWN,  VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_LEFT,  VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_RIGHT, VDP_MAGENTA,     VDP_BLACK);       //Snake head
}

bool menu() {
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();
    char _score_str[10];
    sprintf(_score_str, "SCORE: %03d", score);
    vdp_clearScreen();
    vdp_setCursor2(16-(12/2),4);
    vdp_print("SNAKE - V3.0");
    vdp_setCursor2(16-(17/2),5);
    vdp_print("BY PRODUCTIONDAVE");
    vdp_setCursor2(16-(13/2),8);
    vdp_print("JOYSTICK ONLY");
    vdp_setCursor2(16-(12/2),9);
    vdp_print("BTN TO PAUSE");
    vdp_setCursor2(16-(17/2), 12);
    vdp_print("BTN TO PLAY AGAIN");
    vdp_setCursor2(16-(10/2),14);
    vdp_print(_score_str);
    vdp_setCursor2(16-(11/2), 17);
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

void setup_game() {
    vdp_fillScreen(0x00);
    initNABULIBAudio(); // reset to regular music mode
    // set up new game data
    head.x = 15;
    head.y = 20;
    head.dir = HEAD_RIGHT;
    head.pattern = HEAD_RIGHT;
    buffer_head = -2;
    buffer_tail = -2;
    segments = more_segments;
    ticks = 0;
    score = 0;
    game_speed = 10;
    level = 1;
    // line(5,12,27,12);
    new_apple();
}

void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    uint8_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    uint8_t dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    uint8_t err = (dx > dy ? dx : -dy) >> 1;

    while (x0 != x1 || y0 != y1) {
        vdp_setCharAtLocationBuf(x0, y0, '#');
        uint8_t e2 = err;
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

void game() {
    vdp_enableVDPReadyInt();
    while(!quit) {
        //get input - Joystick move in 4 directions. (EASY MODE)
        if(getJoyStatus(0) & Joy_Button) {
            pause = !pause;
        }
        else if(getJoyStatus(0) & Joy_Left && head.dir != HEAD_RIGHT) {
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

        if (ticks % game_speed == 0 && pause == false) {
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
                break;

            uint8_t _next = vdp_getCharAtLocationBuf(head.x, head.y);
            if(_next == APPLE) {
            //Apple check
                segments = more_segments;
                new_apple();
                score ++;
                if (score % 5 == 0 && game_speed > 5) {
                    game_speed --;
                }
                playNoteDelay(1, 43, 15);
                playNoteDelay(2, 43, 15);
            } else if (_next != 0x00) {
                break;
            }

            // update buffer pointers
            if (buffer_head < 0x4000) {
                buffer_head = buffer_head + 2;
            } else {
                buffer_head = 0;
            }
            circularBuffer[buffer_head] = head.x;
            circularBuffer[buffer_head + 1] = head.y;

            if(segments == 0) {
                if(buffer_tail < 0x4000) {
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
        vdp_waitVDPReadyInt();
        vdp_refreshViewPort();
        ticks ++;
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
    uint16_t timer = 0;
    while (timer < 180) { //3 seconds
        vdp_waitVDPReadyInt();
        timer ++;
    }
    vdp_disableVDPReadyInt();  //only use vdp interrupts during game play.
}

void main() {
    init();
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