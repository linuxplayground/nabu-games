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
    uint8_t character = 'A';
    uint8_t ticks = 0;
    uint8_t speed = 1;
    uint8_t x = 0;
    uint8_t y = 0;
    while(true) {
        // Use . (<) to slow down ,(>) to speed up.
        // speed starts at 0 and will be printed to console so you can see it when you exit.
        if(isKeyPressed()) {
            break;
        }
        // not even making a function call.
        // if (ticks % speed == 0) {
            for(y = 0; y<24; y++) {
                for(x=0; x<32; x++) {
                    vdp_setCharAtLocationBuf(x, y, character);
                }
            }

            character ++;
            if (character > 0x7e) {
                character = 0x20;
            }
        // }
        vdp_waitVDPReadyInt();
        vdp_refreshViewPort();
        ticks ++;
    }
}