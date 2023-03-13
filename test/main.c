#define BIN_TYPE BIN_CPM
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR
#define DISABLE_KEYBOARD_INT

#include "../NABULIB/NABU-LIB.h"
#include <arch/z80.h> // for z80_delay_ms()
#include <conio.h>


void main() {
        while (true) {
                uint8_t key = kbhit();
                if (key) {
                        uint8_t k = getch();
                        if (k == 0x1b) {
                                break;
                        }
                        if (k == ',') {
                                printf("LEFT");
                        }
                        if (k == '.') {
                                printf("RIGHT");
                        }
                        if (k == ' ') {
                                printf("SPACE");
                        }
                        printf(" -- %c\n", key);
                } else {
                        printf("%d\n", key);
                }
        }
}