const uint8_t patterns[0x3C0] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 000H, */
        0x00,0x03,0x07,0x0F,0x1F,0x3F,0x3F,0x3F, /*  DB 001H, Shield Stage 1*/
        0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /*  DB 002H, Shield Stage 1*/
        0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0xFC,0xFC, /*  DB 003H, Shield Stage 1*/
        0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F, /*  DB 004H, Shield Stage 1*/
        0xFF,0xFF,0xFF,0xFF,0xC3,0x81,0x81,0x81, /*  DB 005H, Shield Stage 1*/
        0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC, /*  DB 006H, Shield Stage 1*/
        0x00,0x03,0x06,0x0D,0x1E,0x2B,0x37,0x3B, /*  DB 007H, Shield Stage 2*/
        0x00,0xFB,0x7D,0xBB,0xDB,0xA7,0xDB,0xDD, /*  DB 008H, Shield Stage 2*/
        0x00,0xC0,0xA0,0x70,0xB8,0xB4,0xCC,0xB4, /*  DB 009H, Shield Stage 2*/
        0x2F,0x1F,0x2F,0x37,0x34,0x1B,0x27,0x3B, /*  DB 00AH, Shield Stage 2*/
        0xAF,0x22,0xDD,0xBE,0x43,0x81,0x81,0x81, /*  DB 00BH, Shield Stage 2*/
        0xF4,0x74,0xA8,0xDC,0xDC,0xBC,0xCC,0xF4, /*  DB 00CH, Shield Stage 2*/
        0x00,0x03,0x05,0x09,0x18,0x37,0x13,0x36, /*  DB 00DH, Shield Stage 3*/
        0x00,0x93,0xDA,0xEA,0x35,0x65,0xFF,0x45, /*  DB 00EH, Shield Stage 3*/
        0x00,0xC0,0xA0,0xD0,0x18,0xDC,0xB4,0x24, /*  DB 00FH, Shield Stage 3*/
        0x2B,0x23,0x31,0x37,0x1C,0x09,0x2B,0x3C, /*  DB 010H, Shield Stage 3*/
        0xD9,0x4B,0x24,0xDB,0xC2,0x01,0x80,0x01, /*  DB 011H, Shield Stage 3*/
        0xB0,0x3C,0x4C,0xD8,0x70,0xB0,0xC8,0x6C, /*  DB 012H, Shield Stage 3*/
        0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 013H, Line at top of byte - 1*/
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 014H, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 015H, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 016H, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 017H, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 018H, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 019H, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 01AH, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 01BH, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 01CH, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 01DH, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 01EH, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 01FH, */

        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 020H, */
        0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00, /*  DB 021H, */
        0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 022H, */
        0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00, /*  DB 023H, */
        0x7C,0xD6,0xD0,0x7C,0x16,0xD6,0x7C,0x00, /*  DB 024H, */
        0xC6,0xCE,0x1C,0x38,0x70,0xE6,0xC6,0x00, /*  DB 025H, */
        0x38,0x6C,0x6C,0x30,0xDA,0xCC,0x76,0x00, /*  DB 026H, */
        0x38,0x38,0x18,0x70,0x00,0x00,0x00,0x00, /*  DB 027H, */
        0x18,0x30,0x60,0x60,0x60,0x30,0x18,0x00, /*  DB 028H, */
        0x60,0x30,0x18,0x18,0x18,0x30,0x60,0x00, /*  DB 029H, */
        0x92,0x54,0x38,0xFE,0x38,0x54,0x92,0x00, /*  DB 02AH, */
        0x00,0x18,0x18,0x7E,0x7E,0x18,0x18,0x00, /*  DB 02BH, */
        0x00,0x00,0x00,0x00,0x38,0x38,0x18,0x70, /*  DB 02CH, */
        0x00,0x00,0x00,0x3C,0x3C,0x00,0x00,0x00, /*  DB 02DH, */
        0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C,0x00, /*  DB 02EH, */
        0x06,0x0E,0x1C,0x38,0x70,0xE0,0xC0,0x00, /*  DB 02FH, */
        0x7C,0xC6,0xCE,0xFE,0xF6,0xC6,0x7C,0x00, /*  DB 030H, */
        0x18,0x38,0x18,0x18,0x18,0x18,0x3C,0x00, /*  DB 031H, */
        0x7C,0xCE,0xCE,0x1C,0x38,0x70,0xFE,0x00, /*  DB 032H, */
        0x7C,0xC6,0x06,0x1C,0x06,0xC6,0x7C,0x00, /*  DB 033H, */
        0x0C,0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x00, /*  DB 034H, */
        0xFE,0xC0,0xFC,0xC6,0x06,0xC6,0x7C,0x00, /*  DB 035H, */
        0x7C,0xC6,0xC0,0xFC,0xC6,0xC6,0x7C,0x00, /*  DB 036H, */
        0xFE,0xCE,0x0C,0x18,0x38,0x38,0x38,0x00, /*  DB 037H, */
        0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00, /*  DB 038H, */
        0x7C,0xC6,0xC6,0x7E,0x06,0xC6,0x7C,0x00, /*  DB 039H, */
        0x38,0x38,0x38,0x00,0x38,0x38,0x38,0x00, /*  DB 03AH, */
        0x38,0x38,0x38,0x00,0x38,0x18,0x18,0x00, /*  DB 03BH, */
        0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00, /*  DB 03CH, */
        0x00,0x7C,0x7C,0x00,0x7C,0x7C,0x00,0x00, /*  DB 03DH, */
        0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00, /*  DB 03EH, */
        0x7C,0xC6,0xCC,0x18,0x18,0x00,0x18,0x00, /*  DB 03FH, */
        0x7C,0x82,0xBA,0x8A,0xBA,0xAA,0x94,0x00, /*  DB 040H, */
        0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0x00, /*  DB 041H, */
        0xFC,0xC6,0xC6,0xFC,0xC6,0xC6,0xFC,0x00, /*  DB 042H, */
        0x7C,0xC6,0xC0,0xC0,0xC0,0xC6,0x7C,0x00, /*  DB 043H, */
        0xF8,0xCC,0xC6,0xC6,0xC6,0xCC,0xF8,0x00, /*  DB 044H, */
        0xFE,0xC0,0xC0,0xF8,0xC0,0xC0,0xFE,0x00, /*  DB 045H, */
        0xFE,0xC0,0xC0,0xF8,0xC0,0xC0,0xC0,0x00, /*  DB 046H, */
        0x7C,0xC6,0xC0,0xC0,0xCE,0xC6,0x7E,0x00, /*  DB 047H, */
        0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00, /*  DB 048H, */
        0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00, /*  DB 049H, */
        0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00, /*  DB 04AH, */
        0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00, /*  DB 04BH, */
        0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0x00, /*  DB 04CH, */
        0xC6,0xFE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00, /*  DB 04DH, */
        0xE6,0xE6,0xF6,0xD6,0xDE,0xCE,0xCE,0x00, /*  DB 04EH, */
        0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00, /*  DB 04FH, */
        0xFC,0xC6,0xC6,0xFC,0xC0,0xC0,0xC0,0x00, /*  DB 050H, */
        0x7C,0xC6,0xC6,0xC6,0xDE,0xCC,0x7E,0x00, /*  DB 051H, */
        0xFC,0xC6,0xC6,0xFC,0xD8,0xCC,0xC6,0x00, /*  DB 052H, */
        0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00, /*  DB 053H, */
        0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00, /*  DB 054H, */
        0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00, /*  DB 055H, */
        0xC6,0xC6,0xC6,0x6C,0x6C,0x38,0x38,0x00, /*  DB 056H, */
        0xC6,0xC6,0xC6,0xD6,0xD6,0xFE,0x6C,0x00, /*  DB 057H, */
        0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00, /*  DB 058H, */
        0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00, /*  DB 059H, */
        0xFE,0x0E,0x1C,0x38,0x70,0xE0,0xFE,0x00, /*  DB 05AH, */
        0x78,0x60,0x60,0x60,0x60,0x60,0x78,0x00, /*  DB 05BH, */
        0x3C,0x42,0x99,0xA1,0xA1,0x99,0x42,0x3C, /*  DB 05CH, */
        0x78,0x18,0x18,0x18,0x18,0x18,0x78,0x00, /*  DB 05DH, */
        0x38,0x6C,0xC6,0x00,0x00,0x00,0x00,0x00, /*  DB 05EH, */
        0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x7E, /*  DB 05FH, */
        0x1E,0xFF,0xCC,0xFF,0xFF,0x12,0x21,0xC0, /*  DB 060H, Alien 1_0L*/
        0x00,0xC0,0xC0,0xC0,0xC0,0x00,0x00,0xC0, /*  DB 061H, Alien 1_0R*/
        0x07,0x3F,0x33,0x3F,0x3F,0x04,0x08,0x0C, /*  DB 062H, Alien 1_1L*/
        0x80,0xF0,0x30,0xF0,0xF0,0x80,0x40,0xC0, /*  DB 063H, Alien 1_1R*/
        0x01,0x0F,0x0C,0x0F,0x0F,0x01,0x02,0x0C, /*  DB 064H, Alien 1_2L*/
        0xE0,0xFC,0xCC,0xFC,0xFC,0x20,0x10,0x0C, /*  DB 065H, Alien 1_2R*/
        0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x00, /*  DB 066H, Alien 1_3L*/
        0x78,0xFF,0x33,0xFF,0xFF,0x48,0x84,0xCC, /*  DB 067H, Alien 1_3R*/
        0x63,0x22,0x3E,0x6B,0xFF,0xBE,0xA2,0x36, /*  DB 068H, Alien 2_0L*/
        0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00, /*  DB 069H, Alien 2_0R*/
        0x18,0x08,0x2F,0x2A,0x3F,0x0F,0x08,0x30, /*  DB 06AH, Alien 2_1L*/
        0xC0,0x80,0xA0,0xA0,0xE0,0x80,0x80,0x60, /*  DB 06BH, Alien 2_1R*/
        0x06,0x02,0x03,0x06,0x0F,0x0B,0x0A,0x03, /*  DB 06CH, Alien 2_2L*/
        0x30,0x20,0xE0,0xB0,0xF8,0xE8,0x28,0x60, /*  DB 06DH, Alien 2_2R*/
        0x01,0x00,0x02,0x02,0x03,0x00,0x00,0x03, /*  DB 06EH, Alien 2_3L*/
        0x8C,0x88,0xFA,0xAA,0xFE,0xF8,0x88,0x06, /*  DB 06FH, Alien 2_3R*/
        0x08,0x1C,0x3E,0x6B,0x7F,0x14,0x22,0x41, /*  DB 070H, Alien 3_0L*/
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  DB 071H, Alien 3_0R*/
        0x02,0x07,0x0F,0x1A,0x1F,0x05,0x08,0x05, /*  DB 072H, Alien 3_1L*/
        0x00,0x00,0x80,0xC0,0xC0,0x00,0x80,0x00, /*  DB 073H, Alien 3_1R*/
        0x00,0x01,0x03,0x06,0x07,0x01,0x02,0x04, /*  DB 074H, Alien 3_2L*/
        0x80,0xC0,0xE0,0xB0,0xF0,0x40,0x20,0x10, /*  DB 075H, Alien 3_2R*/
        0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00, /*  DB 076H, Alien 3_3L*/
        0x20,0x70,0xF8,0xAC,0xFC,0x50,0x88,0x50  /*  DB 077H, Alien 3_3R*/
};

typedef struct invader {
        uint8_t lp;             // lp stands for left pattern name
        uint8_t tx;             // tile x
        uint8_t ty;             // tile y
        uint8_t px;             // pixel x
} invader;

invader invaders[5][16];

const invader invaders_default[5][16] = {
        {
                {0x70,1 ,2,  8},
                {0x70,3 ,2, 16},
                {0x70,5 ,2, 24},
                {0x70,7 ,2, 32},
                {0x70,9 ,2, 40},
                {0x70,11,2, 48},
                {0x70,13,2, 56},
                {0x70,15,2, 64},
                {0x70,17,2, 72},
                {0x70,19,2, 80},
                {0x70,21,2, 88},
                {0   ,23,2, 96},
                {0   ,25,2,104},
                {0   ,27,2,112},
                {0   ,29,2,120},
                {0   ,31,2,128}
        },
        {
                {0x68, 1 ,4,  8},
                {0x68, 3 ,4, 16},
                {0x68, 5 ,4, 24},
                {0x68, 7 ,4, 32},
                {0x68, 9 ,4, 40},
                {0x68, 11,4, 48},
                {0x68, 13,4, 56},
                {0x68, 15,4, 64},
                {0x68, 17,4, 72},
                {0x68, 19,4, 80},
                {0x68, 21,4, 88},
                {0    ,23,4, 96},
                {0    ,25,4,104},
                {0    ,27,4,112},
                {0    ,29,4,120},
                {0    ,31,4,128}
        },
        {
                {0x68, 1 ,6,  8},
                {0x68, 3 ,6, 16},
                {0x68, 5 ,6, 24},
                {0x68, 7 ,6, 32},
                {0x68, 9 ,6, 40},
                {0x68, 11,6, 48},
                {0x68, 13,6, 56},
                {0x68, 15,6, 64},
                {0x68, 17,6, 72},
                {0x68, 19,6, 80},
                {0x68, 21,6, 88},
                {0    ,23,6, 96},
                {0    ,25,6,104},
                {0    ,27,6,112},
                {0    ,29,6,120},
                {0    ,31,6,128}
        },
        {
                {0x60, 1 ,8,  8},
                {0x60, 3 ,8, 16},
                {0x60, 5 ,8, 24},
                {0x60, 7 ,8, 32},
                {0x60, 9 ,8, 40},
                {0x60, 11,8, 48},
                {0x60, 13,8, 56},
                {0x60, 15,8, 64},
                {0x60, 17,8, 72},
                {0x60, 19,8, 80},
                {0x60, 21,8, 88},
                {0    ,23,8, 96},
                {0    ,25,8,104},
                {0    ,27,8,112},
                {0    ,29,8,120},
                {0    ,31,8,128}
        },
        {
                {0x60, 1 ,10,  8},
                {0x60, 3 ,10, 16},
                {0x60, 5 ,10, 24},
                {0x60, 7 ,10, 32},
                {0x60, 9 ,10, 40},
                {0x60, 11,10, 48},
                {0x60, 13,10, 56},
                {0x60, 15,10, 64},
                {0x60, 17,10, 72},
                {0x60, 19,10, 80},
                {0x60, 21,10, 88},
                {0    ,23,10, 96},
                {0    ,25,10,104},
                {0    ,27,10,112},
                {0    ,29,10,120},
                {0    ,31,10,128}
        }
};

const uint8_t shield_layout[2][21] = {
        {0x01,0x02,0x03,0,0,0,0x01,0x02,0x03,0,0,0,0x01,0x02,0x03,0,0,0,0x01,0x02,0x03},
        {0x04,0x05,0x06,0,0,0,0x04,0x05,0x06,0,0,0,0x04,0x05,0x06,0,0,0,0x04,0x05,0x06}
};

const uint8_t sprites[32*5] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,        // empty
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

        0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x02,        // player - 1
        0x02,0x26,0x26,0x2F,0x3E,0x7C,0x70,0x40,
        0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x40,
        0x40,0x64,0x64,0xF4,0x7C,0x3E,0x0E,0x02,

        0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,         // bullet - 2
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,        // bomb - 3
        0x00,0x00,0x40,0x80,0x40,0x80,0x40,0x80,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

        0x05,0x00,0x22,0x00,0x10,0xA5,0x02,0x45,        // explosion - 4
        0x20,0x52,0x08,0xA0,0x15,0x60,0x00,0x00,
        0x00,0x00,0x20,0x00,0x40,0x28,0x00,0x80,
        0x20,0x50,0x80,0x28,0x40,0x30,0x00,0x00
};

const uint8_t set_bit_mask[8] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001
};

const uint8_t clear_bit_mask[8] = {
        0b01111111,
        0b10111111,
        0b11011111,
        0b11101111,
        0b11110111,
        0b11111011,
        0b11111101,
        0b11111110
};

#define PLAYER 1
#define BULLET 2
#define BOMB 3
#define EXPLODE 4

#define EMPTY_SPRITE 0
#define PLAYER_SPRITE 1
#define BULLET_SPRITE 2
#define BOMB_SPRITE 3
#define EXPLODE_SPRITE 4

// game settings
#define DEFAULT_GAME_SPEED 8
#define PLAYER_SPEED 2
#define BULLET_SPEED 3
#define BOMB_SPEED  3
#define EXPLODE_FRAMES 10

int8_t dir;
int8_t new_dir;
uint8_t running;
uint8_t game_speed;
uint8_t ticks;
uint8_t max_invaders;
uint8_t drop_flag;
uint8_t top_row;
uint8_t bottom_row;
uint8_t num_rows;
uint8_t playerx;
uint8_t update_player;
uint8_t bulletx;
uint8_t bullety;
uint8_t bullet_active;
uint8_t bullet_t_x, bullet_t_y;
uint8_t bullet_note;
uint8_t first_bomb_delay;
uint8_t bombx;
uint8_t bomby;
uint8_t bomb_active;
uint8_t bomb_t_x, bomb_t_y;
uint8_t explode_active;
uint16_t score;
uint16_t high_score;
bool play_again;
bool level_up;

char tb[6] = { 0 };