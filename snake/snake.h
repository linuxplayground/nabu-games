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
uint8_t level;
bool pause = false;
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

void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);