#define BIN_TYPE BIN_CPM
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

// Showing how to access ui via cp/m
void main() {
        while (true) {
                if (isKeyPressed() ) {
                        uint8_t key = getChar();
                        if (key == ',') {
                                vdp_print("LEFT");
                        }
                        if (key == '.') {
                                vdp_print("RIGHT");
                        }
                        if (key == ' ') {
                                vdp_print("SPACE");
                        }
                        if (key == 0x1b) {
                                vdp_print("QUITTING");
                                break;
                        }
                        sprintf(tb, " -- %c\n", key);
                        vdp_print(tb);
                }
        }
}
