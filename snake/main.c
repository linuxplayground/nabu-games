#define BIN_TYPE BIN_CPM
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

// These patterns have been redefined in patterns.h
#define HEAD_UP    'a'
#define HEAD_DOWN  'b'
#define HEAD_LEFT  'c'
#define HEAD_RIGHT 'd'

#include "../NABULIB/NABU-LIB.h"
#include "patterns.h"
// #include "colors.h"
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
uint8_t applechar = 'e';
int headIndex;
int tailIndex;
int8_t circularBuffer[1536];
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
            else if (key == ',') {          // LEFT ARROW
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
            else if (key == '.') {          // RIGHT ARROW
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
        } else if (next != 0x00) {
            running = false;
        }
        
        // game speed
        z80_delay_ms(game_speed);

        //wait for vsync
        if (running) {
            // move the head index along by 2 and roll over if buffer overflow
            if (headIndex <= 1534) {
                headIndex = headIndex + 2;
            } else {
                headIndex = 0;
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
                if (tailIndex <= 1534) {
                    tailIndex = tailIndex + 2;
                } else {
                    tailIndex = -2;
                }
            } else {
                more_segments = more_segments - 1;
            }
            // music play note every 3rd game tick.
            if (ticks % 3 == 0)
                playNoteDelay(1, song[songPos++ % sizeof(song)], 10);
        }
    }
}

void replay_menu() {

    vdp_setCursor2(11, 12);
    vdp_print("SCORE: ");
    
    sprintf(tb, "%03u", score);
    vdp_print(tb);

    vdp_setCursor2(9, 14);
    vdp_print("AGAIN: (Y/N) ?");

    while (true) {
        
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
    vdp_setCursor2(6, 6);
    vdp_print("PRESS SPACE TO START");
    vdp_setCursor2(7, 10);
    vdp_print("KEYBOARD:");
    vdp_setCursor2(7, 12);
    vdp_print("LEFT     ,(<)");
    vdp_setCursor2(7, 13);
    vdp_print("RIGHT    .(>)");
    
    while (true) {
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
    vdp_loadASCIIFont(ASCII);
    // vdp_loadColorTable(COLOR, sizeof(COLOR));            //Load the default color table. (dark blue on black)

    //Load same colour into every colour table cell.
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i<_vdpColorTableSize; i++) {
        IO_VDPDATA = 0x41;                                  // Dark blue on black
    }

    vdp_setBackDropColor(VDP_DARK_YELLOW);                  //Set border color

    // overwrite special colours.
    vdp_colorizePattern('e', VDP_LIGHT_GREEN, VDP_BLACK);   //Apple
    vdp_colorizePattern('a', VDP_MAGENTA, VDP_BLACK);       //Snake head
    vdp_colorizePattern('b', VDP_MAGENTA, VDP_BLACK);       //Snake head
    vdp_colorizePattern('c', VDP_MAGENTA, VDP_BLACK);       //Snake head
    vdp_colorizePattern('d', VDP_MAGENTA, VDP_BLACK);       //Snake head

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
