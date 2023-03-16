#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "../NABULIB/NABU-LIB.h"
#include "../NABULIB/NabuTracker.h"
#include "../include/patterns.h"

// These patterns have been redefined in patterns.h
#define HEAD_UP    0x01
#define HEAD_DOWN  0x02
#define HEAD_LEFT  0x03
#define HEAD_RIGHT 0x04
#define APPLE      0x05

// Globals
int16_t buffer_head, buffer_tail;
uint8_t score;
uint8_t more_segments=2;  //Number of segments to add
uint8_t segments;         //Variable contianing number of segments to add during current game tick
uint8_t ticks;
uint8_t game_speed;
uint8_t applechar = APPLE;
bool quit = false;
int8_t circularBuffer[0x4000];

//Music score
uint16_t music[] = {
    0, NT_NOTE_ON, 0, 20, 10,
    1, NT_NOTE_OFF, 0,
    2, NT_NOTE_ON, 0, 10, 10,
    3, NT_NOTE_OFF, 0,
    4, NT_NOTE_ON, 0, 20, 10,
    5, NT_NOTE_OFF, 0,
    6, NT_NOTE_ON, 0, 10, 10,
    7, NT_NOTE_OFF, 0,
    8, NT_NOTE_ON, 0, 15, 10,
    9, NT_NOTE_OFF, 0,
    10, NT_NOTE_ON, 0, 20, 10,
    11, NT_NOTE_OFF, 0,
    12, NT_NOTE_ON, 0, 10, 10,
    13, NT_NOTE_OFF, 0,
    14, NT_NOTE_ON, 0, 30, 10,
    15, NT_NOTE_OFF, 0,
    16, NT_LOOP
};

struct {
    int8_t x;
    int8_t y;
    uint8_t dir;
    uint8_t pattern;
} head;

struct {
    int8_t x;
    int8_t y;
} apple;

void init() {
    initNABULib();
    initNABULIBAudio();
    nt_init(music);
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false);
    vdp_enableVDPReadyInt();
    vdp_loadPatternTable(FAT,0x330);
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
}

bool menu() {
    char _score_str[10];
    sprintf(_score_str, "SCORE: %03d", score);

    vdp_clearScreen();
    vdp_setCursor2(16-(12/2),4);
    vdp_print("SNAKE - V2.1");
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
        if (vdp_getCharAtLocationVRAM(x,y) == 0x00) {
            _taken = false;
            apple.x = x;
            apple.y = y;
        }      
    }
    vdp_writeCharAtLocation(apple.x, apple.y, applechar);
}


void setup_game() {
    vdp_fillScreen(0x00);
    initNABULIBAudio(); // reset to regular music mode
    // set up new game data
    head.x = 15;
    head.y = 12;
    head.dir = HEAD_RIGHT;
    head.pattern = HEAD_RIGHT;
    buffer_head = -2;
    buffer_tail = -2;
    segments = more_segments;
    ticks = 0;
    score = 0;
    game_speed = 10;
    new_apple();
}

void game() {
    while(!quit) {
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
                break;

            uint8_t _next = vdp_getCharAtLocationVRAM(head.x, head.y);
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
            vdp_writeCharAtLocation(head.x, head.y, head.pattern);
            //clear the tail
            vdp_writeCharAtLocation(circularBuffer[buffer_tail], circularBuffer[buffer_tail + 1], 0x00);
            nt_handleNote();
        }
        vdp_waitVDPReadyInt();
        ticks ++;
    }
}

void main() {
    init();
    while(menu()) {
        setup_game();
        game();
        // play crash sound
        ayWrite(6,  0x0f);
        ayWrite(7,  0b11000111);
        ayWrite(8,  0x1f);
        ayWrite(9,  0x1f);
        ayWrite(10, 0x1f);
        ayWrite(11, 0xa0);
        ayWrite(12, 0x40);
        ayWrite(13, 0x00);
    };
    vdp_disableVDPReadyInt();
    #if BIN_TYPE == BIN_HOMEBREW
    __asm
        di
        rst 0x00
    __endasm;
    #endif
}