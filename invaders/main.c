#define BIN_TYPE BIN_CPM
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "../NABULIB/NABU-LIB.h"
#include "../NABULIB/NabuTracker.h"
#include "../include/patterns.h"
#include "sprites.h"
#include <arch/z80.h> // for z80_delay_ms()

#define SPRITE_NAME_BLANK   0
#define SPRITE_NAME_ALIEN_1 1
#define SPRITE_NAME_ALIEN_2 2
#define SPRITE_NAME_ALIEN_3 3
#define SPRITE_NAME_ALIEN_4 4
#define SPRITE_NAME_PLAYER  5

struct Bullet {
    uint8_t x;
    uint8_t y;
};

struct {
    uint8_t x;
    uint8_t y;
    uint8_t sprite_id;
    struct Bullet _bullet;
    bool vdp_ready;
} player = {0,0,0,{0,0},false};

void init_display() {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, true, false, false); //uint8_t bgColor, bool bigSprites, bool scaleSprites, bool autoScroll
    vdp_enableVDPReadyInt();
    vdp_loadPatternTable(FAT,0x330);
    //Load same colour into every colour table cell.
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i<_vdpColorTableSize; i++) {
        IO_VDPDATA = 0x41;                  // Dark blue on black
    }
    vdp_loadSpritePatternNameTable(6, SPRITE_DATA);
}

void setup_player() {
    player.x = 120;
    player.y = 176;
    player.sprite_id = vdp_spriteInit(0, SPRITE_NAME_PLAYER, 10, 10, VDP_WHITE);
    player._bullet.x = 128;
    player._bullet.y = 176;
    player.vdp_ready = false;
}

void main() {
    init_display();
    setup_player();

    while(true) {
        if (isKeyPressed()){
            uint8_t key = getChar();
            if (key == 'q') {
                break;
            } else if (key == ',') {
                player.x --;
                player.vdp_ready = true;
            } else if (key == '.') {
                player.x ++;
                player.vdp_ready = true;
            }
        }
        vdp_waitVDPReadyInt();
        if (player.vdp_ready) {
            vdp_setSpritePosition(player.sprite_id, player.x, player.y);
            player.vdp_ready = false;
        }
    }
}