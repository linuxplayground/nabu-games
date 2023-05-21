const uint8_t patterns[344] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      //0 blank
        0x1E,0xFF,0xCC,0xFF,0xFF,0x12,0x21,0xC0,      //1 offset0
        0x00,0xC0,0xC0,0xC0,0xC0,0x00,0x00,0xC0,      //2
        0x07,0x3F,0x33,0x3F,0x3F,0x04,0x08,0x0C,      //3 offset1
        0x80,0xF0,0x30,0xF0,0xF0,0x80,0x40,0xC0,      //4
        0x01,0x0F,0x0C,0x0F,0x0F,0x01,0x02,0x0C,      //5 offset2
        0xE0,0xFC,0xCC,0xFC,0xFC,0x20,0x10,0x0C,      //6
        0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x00,      //7 offset3
        0x78,0xFF,0x33,0xFF,0xFF,0x48,0x84,0xCC,      //8

        0x63,0x22,0x3E,0x6B,0xFF,0xBE,0xA2,0x36,      //9
        0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,      //10
        0x18,0x08,0x2F,0x2A,0x3F,0x0F,0x08,0x30,      //11
        0xC0,0x80,0xA0,0xA0,0xE0,0x80,0x80,0x60,      //12
        0x06,0x02,0x03,0x06,0x0F,0x0B,0x0A,0x03,      //13
        0x30,0x20,0xE0,0xB0,0xF8,0xE8,0x28,0x60,      //14
        0x01,0x00,0x02,0x02,0x03,0x00,0x00,0x03,      //15
        0x8C,0x88,0xFA,0xAA,0xFE,0xF8,0x88,0x06,      //16

        0x08,0x1C,0x3E,0x6B,0x7F,0x14,0x22,0x41,      //17
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      //18
        0x02,0x07,0x0F,0x1A,0x1F,0x05,0x08,0x05,      //19
        0x00,0x00,0x80,0xC0,0xC0,0x00,0x80,0x00,      //20
        0x00,0x01,0x03,0x06,0x07,0x01,0x02,0x04,      //21
        0x80,0xC0,0xE0,0xB0,0xF0,0x40,0x20,0x10,      //22
        0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,      //23
        0x20,0x70,0xF8,0xAC,0xFC,0x50,0x88,0x50,      //24

        0x00,0x03,0x07,0x0F,0x1F,0x3F,0x3F,0x3F,      //25      shield new
        0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,      //26
        0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0xFC,0xFC,      //27
        0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,      //28
        0xFF,0xFF,0xFF,0xFF,0xC3,0x81,0x81,0x81,      //29
        0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,      //30

        0x00,0x03,0x06,0x0D,0x1E,0x2B,0x37,0x3B,      //31      shield stage 1
        0x00,0xFB,0x7D,0xBB,0xDB,0xA7,0xDB,0xDD,      //32
        0x00,0xC0,0xA0,0x70,0xB8,0xB4,0xCC,0xB4,      //33
        0x2F,0x1F,0x2F,0x37,0x34,0x1B,0x27,0x3B,      //34
        0xAF,0x22,0xDD,0xBE,0x43,0x81,0x81,0x81,      //35
        0xF4,0x74,0xA8,0xDC,0xDC,0xBC,0xCC,0xF4,      //36

        0x00,0x03,0x05,0x09,0x18,0x37,0x13,0x36,      //37      shield stage 2
        0x00,0x93,0xDA,0xEA,0x35,0x65,0xFF,0x45,      //38
        0x00,0xC0,0xA0,0xD0,0x18,0xDC,0xB4,0x24,      //39
        0x2B,0x23,0x31,0x37,0x1C,0x09,0x2B,0x3C,      //40
        0xD9,0x4B,0x24,0xDB,0xC2,0x01,0x80,0x01,      //41
        0xB0,0x3C,0x4C,0xD8,0x70,0xB0,0xC8,0x6C       //42
};

typedef struct invader {
        uint8_t lp;             // lp stands for left pattern name
        uint8_t tx;             // tile x
        uint8_t ty;             // tile y
        uint8_t px;             // pixel x
} invader;

invader invaders[5][16] = {
        {
                {17,1 ,2,  8},
                {17,3 ,2, 16},
                {17,5 ,2, 24},
                {17,7 ,2, 32},
                {17,9 ,2, 40},
                {17,11,2, 48},
                {17,13,2, 56},
                {17,15,2, 64},
                {17,17,2, 72},
                {17,19,2, 80},
                {17,21,2, 88},
                {0 ,23,2, 96},
                {0 ,25,2,104},
                {0 ,27,2,112},
                {0 ,29,2,120},
                {0 ,31,2,128}
        },
        {
                {9, 1 ,4,  8},
                {9, 3 ,4, 16},
                {9, 5 ,4, 24},
                {9, 7 ,4, 32},
                {9, 9 ,4, 40},
                {9, 11,4, 48},
                {9, 13,4, 56},
                {9, 15,4, 64},
                {9, 17,4, 72},
                {9, 19,4, 80},
                {9, 21,4, 88},
                {0 ,23,4, 96},
                {0 ,25,4,104},
                {0 ,27,4,112},
                {0 ,29,4,120},
                {0 ,31,4,128}
        },
        {
                {9, 1 ,6,  8},
                {9, 3 ,6, 16},
                {9, 5 ,6, 24},
                {9, 7 ,6, 32},
                {9, 9 ,6, 40},
                {9, 11,6, 48},
                {9, 13,6, 56},
                {9, 15,6, 64},
                {9, 17,6, 72},
                {9, 19,6, 80},
                {9, 21,6, 88},
                {0 ,23,6, 96},
                {0 ,25,6,104},
                {0 ,27,6,112},
                {0 ,29,6,120},
                {0 ,31,6,128}
        },
        {
                {1, 1 ,8,  8},
                {1, 3 ,8, 16},
                {1, 5 ,8, 24},
                {1, 7 ,8, 32},
                {1, 9 ,8, 40},
                {1, 11,8, 48},
                {1, 13,8, 56},
                {1, 15,8, 64},
                {1, 17,8, 72},
                {1, 19,8, 80},
                {1, 21,8, 88},
                {0 ,23,8, 96},
                {0 ,25,8,104},
                {0 ,27,8,112},
                {0 ,29,8,120},
                {0 ,31,8,128}
        },
        {
                {1, 1 ,10,  8},
                {1, 3 ,10, 16},
                {1, 5 ,10, 24},
                {1, 7 ,10, 32},
                {1, 9 ,10, 40},
                {1, 11,10, 48},
                {1, 13,10, 56},
                {1, 15,10, 64},
                {1, 17,10, 72},
                {1, 19,10, 80},
                {1, 21,10, 88},
                {0 ,23,10, 96},
                {0 ,25,10,104},
                {0 ,27,10,112},
                {0 ,29,10,120},
                {0 ,31,10,128}
        }
};

const uint8_t shield_layout[2][21] = {
        {25,26,27,0,0,0,25,26,27,0,0,0,25,26,27,0,0,0,25,26,27},
        {28,29,30,0,0,0,28,29,30,0,0,0,28,29,30,0,0,0,28,29,30}
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
