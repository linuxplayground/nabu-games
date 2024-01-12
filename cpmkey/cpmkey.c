#include <stdio.h>
#include <stdlib.h>

/*
* ==========================================
* Return true if a key was pressed.
* Does not consume the pressed key.
* ==========================================
*/

//single char stores a key press if one was pressed when isKeyPressed() was called.
uint8_t _kbuf;

/*
* Use stdio getk() which uses BIOS CONST and CONIN to test if a key is ready.
* Store pressed key value into _kbuf;
* return: 1 when a key was pressed, 0 when no key pressed.
*/
uint8_t isKeyPressed(void) {
    uint8_t key = getk();
    if (key) {
        _kbuf = key;
        printf("WRITE: %d - %d - %c\n", _kbuf);
        return 1;
    } else {
        return 0;
    }
}

/*
* CPM esque function to match NABU-LIB getChar() call
* returns whatever is in _kbuf
* resets _kbuf to zero.
*/
uint8_t getChar(void) {
    uint8_t temp = _kbuf;
    _kbuf = 0;
    return temp;
}

uint8_t key;

/*
* Function to test isKeyPressed() and getChar() in CP/M using native C in z88dk.
*/
void main(void) {
    while(1) {
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
