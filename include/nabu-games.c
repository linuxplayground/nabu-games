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

void init() {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, false, false, false, false);
    vdp_loadPatternTable(FAT,0x400);
    uint16_t _vdpColorTableAddr = 0x2000;
    uint16_t _vdpColorTableSize = 0x1800;
    vdp_setPatternColor(0x41);
    vdp_setBackDropColor(VDP_DARK_YELLOW);                         //Set border color
    initNABULIBAudio();
    vdp_enableVDPReadyInt();
}