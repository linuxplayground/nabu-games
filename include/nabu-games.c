//Set all values in the color table to color.
void vdp_setPatternColor(uint8_t color) {
    vdp_setWriteAddress(_vdpColorTableAddr);
    for (uint16_t i = 0; i < 0x1800; i++) {
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

