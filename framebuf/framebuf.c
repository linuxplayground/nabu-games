#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "patterns.h"

void init() {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false, false);
    vdp_enableVDPReadyInt();
    vdp_loadPatternTable(FAT,0x330);
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i<_vdpColorTableSize; i++) {
        IO_VDPDATA = 0x41;                                  // Dark blue on black
    }

    vdp_setBackDropColor(VDP_DARK_YELLOW);                  //Set border color
}

void main(void) {
    init();
    uint8_t character = 0x20;
    uint8_t ticks = 0;
    uint8_t speed = 60;
    while(true) {
        // Use . (<) to slow down ,(>) to speed up.
        // speed starts at 0 and will be printed to console so you can see it when you exit.
        if(isKeyPressed()) {
            break;
        }
        // not even making a function call.
        if (ticks % speed == 0) {
            for(uint16_t i = 0; i < 0x300; i ++) {
                _vdp_textBuffer[i] = character;
            }
            character ++;
            if (character > 0x7e) {
                character = 0x20;
            }
            vdp_waitVDPReadyInt();
            vdp_refreshViewPort();
        }
        vdp_waitVDPReadyInt();
        ticks ++;
        if(ticks > 240)
            ticks = 0;
    }
}