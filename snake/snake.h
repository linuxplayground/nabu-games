// These patterns have been redefined in patterns.h
#define HEAD_UP    0x81
#define HEAD_DOWN  0x82
#define HEAD_LEFT  0x83
#define HEAD_RIGHT 0x84
#define APPLE      0x88

uint8_t snake_pat_up[8] =  {0x18,0x3C,0x3C,0x7E,0x7E,0x7E,0x3C,0x3C};
uint8_t snake_pat_down[8] = {0x3C,0x3C,0x7E,0x7E,0x7E,0x3C,0x3C,0x18};
uint8_t snake_pat_left[8] = {0x00,0x1C,0x7F,0xFF,0xFF,0x7F,0x1C,0x00};
uint8_t snake_pat_right[8] = {0x00,0x38,0xFE,0xFF,0xFF,0xFE,0x38,0x00};
uint8_t snake_pat_apple[8] = {0x04,0x68,0x37,0x7F,0x7F,0x7F,0x7F,0x3E};
//
                    //   0     8    10    18    20    28   30     38    40    48    50    58    60    68   70    78     80    88    90    98    A0    A8    B0    B8    C0    C8    D0    D8    E0   E8    F0    F8
uint8_t colors[32] = {0x51, 0x51, 0x51, 0x51, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x51, 0xD1, 0x31, 0x91, 0xB1, 0x31, 0x61, 0xD1, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41 };

// Globals
int16_t buffer_head, buffer_tail;
uint16_t score;
char score_str[16];
uint16_t high_score;
char high_score_str[16];
uint16_t saved_high_score;

uint8_t more_segments=4;  //Number of segments to add
uint8_t segments;         //Variable contianing number of segments to add during current game tick

uint8_t ticks;
uint8_t game_speed;

uint8_t applechar = APPLE;

uint8_t level;
bool pause = true;
bool crashed = false;

bool inp_joy = false;

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

uint8_t level1[4 + 1] = {
        4,      4,12,27,12
};

uint8_t level2[12 + 1] = {
        12,     4,12,27,12,
                8,3,8,12,
                23,12,23,20
};

uint8_t level3[24 + 1] = {
        24,     4,4,14,4,
                17,4,27,4,
                4,19,14,19,
                17,19,27,19,
                4,4,4,19,
                27,4,27,19
};

uint8_t level4[24 + 1] = {
        24,     3,4,24,4,
                3,4,3,19,
                6,19,27,19,
                27,19,27,4,
                21,4,21,16,
                9,7,9,19,
};


uint8_t *levels[4] = {level1, level2, level3, level4};

#define BUFFSIZE 0x600
int8_t circularBuffer[BUFFSIZE] = { 0 };
