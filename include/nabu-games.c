//Set all values in the color table to color.
void vdp_setPatternColor(uint8_t color) {
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i < 0x400; i++) {
        IO_VDPDATA = color;
        nop();
        nop();
        nop();
        nop();
    }
}

void printAtLocationBuf(uint8_t x, uint8_t y, uint8_t *text) {
    uint16_t offset = y * _vdpCursorMaxXFull + x;
    uint8_t *start = text;

    while (*start != 0x00) {

      _vdp_textBuffer[offset] = *start;
      offset++;
      start++;
    }
}

void centerText(char *text, uint8_t y) {
    printAtLocationBuf(abs(15-(strlen(text)/2)),y,text);
}

void delay(uint8_t frames) {
    while(frames > 0) {
        vdp_waitVDPReadyInt();
        frames --;
    }
}

void hardReset(void) {
    __asm
        // Disable interrupts
        // di
        ld a,0xf3;
        ld (0x8880),a;
        
        // Enable rom shadow
        // lda a,0x02
        ld a,0x3e;
        ld (0x8881),a;
        ld a,0x02;
        ld (0x8882),a;

        // out(0x00),a
        ld a,0xd3;
        ld (0x8883),a;
        ld a,0x00;
        ld (0x8884),a;

        // restart at 0x00
        // rst 0x00
        ld a,0xc7;
        ld (0x8885),a;
        
        jp 0x8880;
    __endasm;
}
