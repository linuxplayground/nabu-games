#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>

/*
* ==========================================
* Return true if a key was pressed.
* Does not consume the pressed key.
* ==========================================
*/

uint8_t _kbuf[256];
uint8_t _krptr;
uint8_t _kwptr;

uint8_t isKeyPressed() {
    uint8_t key = getk();
    if (key) {
        _kbuf[_kwptr] = key;
        printf("WRITE: %d - %d - %c\n", _kbuf[_kwptr], _kwptr, key);
        _kwptr ++;
        return 1;
    } else {
        return 0;
    }
}

uint8_t getChar() {
    while (_krptr == _kwptr);
    uint8_t key = _kbuf[_krptr];
    printf("READ:  %d - %d - %c\n", _kbuf[_krptr], _krptr, key);
    _krptr ++;
    return key;
}

uint8_t key;
void main(void) {
    while(true) {
        if(isKeyPressed()) {
            uint8_t key = getChar();
            if (key == 0x1b) {
                break;
            } else {
                printf("OUTPUT: %c\n", key);
            }
        }
    }
}