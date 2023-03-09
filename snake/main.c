#define BIN_TYPE BIN_CPM
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

// These patterns have been redefined in patterns.h
#define HEAD_UP    0x01
#define HEAD_DOWN  0x02
#define HEAD_LEFT  0x03
#define HEAD_RIGHT 0x04
#define APPLE      0x05

#include "../NABULIB/NABU-LIB.h"
#include "../include/patterns.h"
#include <arch/z80.h> // for z80_delay_ms()

struct {
    uint8_t x;
    uint8_t y;
} apple;

struct {
    int8_t x;
    int8_t y;
    uint8_t dir;
} head;

uint8_t headchar = HEAD_RIGHT;
uint8_t applechar = APPLE;
int headIndex;
int tailIndex;
int8_t circularBuffer[0x4000];
uint8_t note = 43;

int more_segments;
bool running = true;
bool playing = true;
uint8_t game_speed;
const uint8_t song[] = { 20, 10, 20, 10, 15, 20, 10, 30, 23};
uint8_t songPos;
uint8_t ticks;
int16_t score;
uint8_t tb[64] = { 0x00 };
bool crashed;


void new_apple() {
    bool taken = true;
    while(taken) {
        uint8_t x = rand() % 32;
        uint8_t y = rand() % 24;
        if (vdp_getCharAtLocationVRAM(x,y) == 0x00) {
            taken = false;
            apple.x = x;
            apple.y = y;
        }      
    }
    vdp_writeCharAtLocation(apple.x, apple.y, applechar);
}

void initGame() {
    vdp_fillScreen(0x00);

    // set up game data
    head.x = 15;
    head.y = 12;
    head.dir = 1;
    running = true;
    game_speed = 100;
    headchar = HEAD_RIGHT;
    headIndex = -2;
    tailIndex = -2;
    more_segments = 3;
    songPos = 0;
    ticks = 0;
    score = 0;
    crashed = false;
}

void game() {

    // Game loop
    while(running) {
        ticks = ticks + 1;
            // get key
        if(isKeyPressed()) {
            uint8_t key = getChar();
            /*
            * DIR:
            *   0 = LEFT
            *   1 = RIGHT
            *   2 = UP
            *   3 = DOWN
            */
            if (key == 0x1b) {
                running = false;
            }
            else if (key == ',' || getJoyStatus(0) & Joy_Left) {          // LEFT ARROW
                if (head.dir == 0) {
                    head.dir = 3;
                    headchar = HEAD_DOWN;
                }
                else if (head.dir == 1) {
                    head.dir = 2;
                    headchar = HEAD_UP;
                }
                else if (head.dir == 2) {
                    head.dir = 0;
                    headchar = HEAD_LEFT;
                }
                else if (head.dir == 3) {
                    head.dir = 1;
                    headchar = HEAD_RIGHT;
                }
            }
            else if (key == '.' || getJoyStatus(0) & Joy_Right) {          // RIGHT ARROW
                if (head.dir == 0) {
                    head.dir = 2;
                    headchar = HEAD_UP;
                }
                else if (head.dir == 1) {
                    head.dir = 3;
                    headchar = HEAD_DOWN;
                }
                else if (head.dir == 2) {
                    head.dir = 1;
                    headchar = HEAD_RIGHT;
                }
                else if (head.dir == 3) {
                    head.dir = 0;
                    headchar = HEAD_LEFT;
                }
            }
        }
        // move the head
        if (head.dir == 0) {
            head.x --;
        }
        else if (head.dir == 1) {
            head.x ++;
        }
        else if (head.dir == 2) {
            head.y --;
        }
        else if (head.dir == 3) {
            head.y ++;
        }
        
        //check for wall collision
        if(head.x < 0 || head.x > 31 || head.y < 0 || head.y > 23) {
            running = false;
            crashed = true;
        }

        //check for apple collision
        uint8_t next = vdp_getCharAtLocationVRAM(head.x, head.y);
        if (next == applechar){
            playNoteDelay(0, note, 15);
            more_segments = 3;
            score += 1;
            if (score % 5 == 0 && game_speed > 10)
                game_speed = game_speed - 10;
            new_apple();
        } else if (next != 0x00) {  // hit tail
            running = false;
            crashed = true;
        }
        
        // game speed
        z80_delay_ms(game_speed);

        //wait for vsync
        if (running) {
            // move the head index along by 2 and roll over if buffer overflow
            if (headIndex <= 0x4000) {
                headIndex = headIndex + 2;
            } else {
                headIndex = 0;      // Always bring the head to just after the tail.
            }
            // push head location to ring buffer.
            circularBuffer[headIndex] = head.x;
            circularBuffer[headIndex + 1] = head.y;
            
            // vdp_waitVDPReadyInt();
            vdp_writeCharAtLocation(head.x, head.y, headchar);
            // clear the tail at current location.
            vdp_writeCharAtLocation(circularBuffer[tailIndex], circularBuffer[tailIndex + 1], 0x00);

            // if we are not groing, move the tail pointer along
            if (more_segments == 0) {
                if (tailIndex <= 0x4000) {
                    tailIndex = tailIndex + 2;
                } else {
                    tailIndex = 0;
                }
            } else {
                more_segments = more_segments - 1;
            }
            // music play note every 3rd game tick.
            if (ticks % 3 == 0)
                playNoteDelay(2, song[songPos++ % sizeof(song)], 10);
        }
    }
    if (crashed) {
        // play crash sound
        ayWrite(6,  0x0f);
        ayWrite(7,  0b11000111);
        ayWrite(8,  0x1f);
        ayWrite(9,  0x1f);
        ayWrite(10, 0x1f);
        ayWrite(11, 0xa0);
        ayWrite(12, 0x40);
        ayWrite(13, 0x00);
        z80_delay_ms(1500);
        initNABULIBAudio(); // reset to regular music mode
    }

}

void replay_menu() {

    vdp_setCursor2(16-(10/2), 12);
    vdp_setCursor2(16-(10/2), 12);
    vdp_print("SCORE: ");
    
    sprintf(tb, "%03u", score);
    vdp_print(tb);

    vdp_setCursor2(16-(14/2), 14);
    vdp_print("AGAIN: (Y/N) ?");
    vdp_setCursor2(16-(15/2), 16);
    vdp_print("BUTTON TO START");

    while (true) {
        if (getJoyStatus(0) & Joy_Button) {
            playing = true;
            break;
        }
        if(isKeyPressed()) {
            uint8_t res = getChar();
            if (res == 'y') {
                playing = true;
                break;
            } else if (res == 'n') {
                playing = false;
                break;
            }

        }

    }
}

void start_menu() {
    vdp_setCursor2(16-(5/2), 3);
    vdp_print("SNAKE");
    vdp_setCursor2(16-(17/2), 5);
    vdp_print("BY PRODUCTIONDAVE");
    vdp_setCursor2(16-(8/2), 10);
    vdp_print("KEYBOARD");
    vdp_setCursor2(16-(13/2), 12);
    vdp_print("LEFT     ,(<)");
    vdp_setCursor2(16-(13/2), 13);
    vdp_print("RIGHT    .(>)");
    vdp_setCursor2(16-(21/2), 15);
    vdp_print("PRESS SPACE TO START");
    vdp_setCursor2(16-(19/2), 17);
    vdp_print("OR USE THE JOYSTICK");
    vdp_setCursor2(16-(15/2), 18);
    vdp_print("BUTTON TO START");
    
    while (true) {
        if (getJoyStatus(0) & Joy_Button) {
            playing = true;
            break;
        }
        if(isKeyPressed()) {
            uint8_t res = getChar();

            if (res == ' ') {
                playing = true;
                break;
            } else if (res == 0x1b) {
                playing = false;
                break;
            }
        }
    }
}

void main() {
    // set up graphics
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false);                 //uint8_t bgColor, bool bigSprites, bool scaleSprites, bool autoScroll
    // vdp_loadPatternTable(ASCII, 0x400);
    vdp_loadPatternTable(FAT,0x330);
    //Load same colour into every colour table cell.
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i<_vdpColorTableSize; i++) {
        IO_VDPDATA = 0x41;                                  // Dark blue on black
    }

    vdp_setBackDropColor(VDP_DARK_YELLOW);                  //Set border color

    // overwrite special colours.
    vdp_colorizePattern(APPLE,      VDP_LIGHT_GREEN, VDP_BLACK);   //Apple
    vdp_colorizePattern(HEAD_UP,    VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_DOWN,  VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_LEFT,  VDP_MAGENTA,     VDP_BLACK);       //Snake head
    vdp_colorizePattern(HEAD_RIGHT, VDP_MAGENTA,     VDP_BLACK);       //Snake head

    initGame();
    start_menu();
    while(playing) {
        initGame();
        new_apple();
        game();
        z80_delay_ms(1000);
        replay_menu();
    }
    //vdp_disableVDPReadyInt();
}
