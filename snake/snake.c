// vim: set ts=4 sw=4:
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR
#define DEBUG_VDP_INT

#include "NABU-LIB.h"
#include "NabuTracker.h"
#include "nabu-games.h"
#include <strings.h>
#include "snake.h"

void add_pattern(uint8_t* pat, uint8_t pat_num) {
    vdp_setWriteAddress(_vdpPatternGeneratorTableAddr + (pat_num * 8));
    for(uint8_t i=0; i<8; i++) {
        IO_VDPDATA = pat[i];
    }
}
/* Set up the graphics, audio and vdp interrupts.*/
void init(void) {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG1Mode(1, false, false, false, false);
    vdp_loadPatternTable(FAT,0x330);
    vdp_loadColorTable(colors,32);
    vdp_setBackDropColor(VDP_DARK_YELLOW);
    // add snake patterns
    add_pattern(snake_pat_up, 0x81); // up
    add_pattern(snake_pat_down, 0x82); // down
    add_pattern(snake_pat_left, 0x83); // left
    add_pattern(snake_pat_right, 0x84); // right
    add_pattern(snake_pat_apple, 0x88); // apple

    initNABULIBAudio();
    nt_init(music);
    vdp_enableVDPReadyInt();
}

/* Display the menu and then wait for the user to either exit (false) or continue (true)*/
bool menu(void) {
    vdp_waitVDPReadyInt();
    uint8_t tmp = IO_VDPLATCH;  //dummy read
    vdp_clearScreen();

    sprintf(score_str,      "SCORE:      %03d", score);
    sprintf(high_score_str, "HIGH SCORE: %03d", high_score);

    centerText("SNAKE - V3.5",4);
    centerText("BY PRODUCTIONDAVE",5);
    centerText("SPACE = KBD",8);
    centerText("BTN = JOY",9);
    centerText("BTN TO PLAY AGAIN",11);
    centerText(score_str,13);
    centerText(high_score_str,14);
    centerText("ESC TO QUIT",16);

    vdp_waitVDPReadyInt();
    tmp = IO_VDPLATCH;  //dummy read
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort(); 
    while(true) {

        if (getJoyStatus(0) & Joy_Button) {
            inp_joy = true;
            return true;
        }

        uint8_t _key = getKey();
        switch(_key) {
          case 0x1b: return false;
          case 0x20: {
            inp_joy = false;
            return true;
          }
          default: {}
        }
    }
}

/* Locate a new apple on the screen without it overlapping anything already there.*/
void new_apple(void) {
    bool taken = true;
    while(taken) {
        uint8_t x = rand() % 32;
        uint8_t y = rand() % 24;
        if (vdp_getCharAtLocationBuf(x,y) == 0x00) {
            taken = false;
            apple.x = x;
            apple.y = y;
        }
    }
    vdp_setCharAtLocationBuf(apple.x, apple.y, applechar);
}

/* Draw a line between two points - only works for horizontal and vertical lines*/
void line(int x0, int y0, int x1, int y1) {
    if (y0 < y1) {
        for(uint8_t i = y0; i<=y1; i++) {
            vdp_setCharAtLocationBuf(x0, i, 0x15);
        }
    } else if (y1 < y0) {
        for(uint8_t i = y1; i<= y0; i++) {
            vdp_setCharAtLocationBuf(x0, i, 0x15);
        }
    } else if (x0 < x1) {
        for(uint8_t i = x0; i<=x1; i++) {
            vdp_setCharAtLocationBuf(i, y0, 0x15);
        }
    } else {
        for(uint8_t i = x1; i<=x0; i++) {
            vdp_setCharAtLocationBuf(i, y0, 0x15);
        }
    }
}

/* Draw the level onto the screen.*/
void drawLevel(uint8_t *level, uint8_t len) {
    uint8_t *start = level;
    uint8_t *end = start + len;
    uint8_t lines = 0;
    do {
        uint8_t a0 = *start;
        start ++;
        uint8_t b0 = *start;
        start ++;
        uint8_t a1 = *start;
        start ++;
        uint8_t b1 = *start;
        start ++;
        line(a0,b0,a1,b1);
        lines ++;
    } while (start != end);
}

/*Empty out the text frame buffer.*/
void clearTextBuffer(void) {
    for(uint16_t i=0; i<0x300; i++) {
        _vdp_textBuffer[i] = 0x00;
    }
}

/* Sets up a new level */
void setup_level(void) {
    clearTextBuffer();
    //Reset snake length and circ buffer
    head.x = 15;
    head.y = 20;
    head.dir = HEAD_RIGHT;
    head.pattern = HEAD_RIGHT;
    buffer_head = -2;
    buffer_tail = -2;
    ticks = 0;
    segments = more_segments;
    if (level > 0) {
        uint8_t levelsize = *levels[level-1];
        drawLevel(levels[level-1] + 1, levelsize);
    }
    new_apple();
    pause=true;
}

/* Sets up a new game. */
void setup_game(void) {
    clearTextBuffer();
    initNABULIBAudio(); // reset to regular music mode
    score = 0;
    level = 0;
    game_speed = 10;
    crashed = false;
    setup_level();
}

void game(void) {
    uint8_t apples_eaten = 0;
    while(!crashed) {
        if (pause) {
            nt_stopSounds();
            bool flash = false;
            while(true) {
                if (getKey() || getJoyStatus(0) & Joy_Button) {
                    pause = false;
                    break;
                }
                delay(5);
                flash = ~flash;
                if(flash) {
                    vdp_setCharAtLocationBuf(head.x, head.y, head.pattern);
                } else {
                    vdp_setCharAtLocationBuf(head.x, head.y, 0x00);
                }
                vdp_waitVDPReadyInt();
                vdp_refreshViewPort();
            }
            vdp_setCharAtLocationBuf(head.x, head.y, 0x00);
        } else { //if not paused
            if (inp_joy) {
              if( (getJoyStatus(0) & Joy_Left) && (head.dir != HEAD_RIGHT)) {
                  head.dir = HEAD_LEFT;
                  head.pattern = HEAD_LEFT;
              }
              else if( (getJoyStatus(0) & Joy_Up) && (head.dir != HEAD_DOWN)) {
                  head.dir = HEAD_UP;
                  head.pattern = HEAD_UP;
              }
              else if( (getJoyStatus(0) & Joy_Right) && (head.dir != HEAD_LEFT)) {
                  head.dir = HEAD_RIGHT;
                  head.pattern = HEAD_RIGHT;
              }
              else if( (getJoyStatus(0) & Joy_Down) && (head.dir != HEAD_UP)) {
                  head.dir = HEAD_DOWN;
                  head.pattern = HEAD_DOWN;
              }
            } else {
                uint8_t _key = getKey();
                switch(_key) {
                        case 'a':
                        case 'A':
                            head.dir = HEAD_LEFT;
                            head.pattern = HEAD_LEFT;
                            break;
                        case 'w':
                        case 'W':
                            head.dir = HEAD_UP;
                            head.pattern = HEAD_UP;
                            break;
                        case 'd':
                        case 'D':
                            head.dir = HEAD_RIGHT;
                            head.pattern = HEAD_RIGHT;
                            break;
                        case 's':
                        case 'S':
                            head.dir = HEAD_DOWN;
                            head.pattern = HEAD_DOWN;
                            break;
                        default: {}
                }
            }

           /* Regulate the speed of the game */
            if (ticks % game_speed == 0) {
                // move the head
                if (head.dir == HEAD_LEFT) {
                    head.x --;
                }
                else if (head.dir == HEAD_UP) {
                    head.y --;
                }
                else if (head.dir == HEAD_RIGHT) {
                    head.x ++;
                }
                else if (head.dir == HEAD_DOWN) {
                    head.y ++;
                }
                //check for collisions
                //Border Collisions first
                if(head.x < 0 || head.x > 31 || head.y < 0 || head.y > 23) {
                    crashed = true;
                }
                uint8_t _next = vdp_getCharAtLocationBuf(head.x, head.y);
                if(_next == APPLE) {
                //Apple check
                    segments = more_segments;
                    new_apple();
                    apples_eaten ++;
                    score = score + abs(apples_eaten / 10) + 1;

                    if (apples_eaten % 10 == 0 && apples_eaten > 20 && game_speed > 5) {
                        game_speed --;
                    }
                    if (apples_eaten % 10 == 0) {
                        level ++;
                        playNoteDelay(1,65,15);
                        playNoteDelay(2,65,15);
                        delay(15);
                        playNoteDelay(1, 71, 15);
                       playNoteDelay(2, 71, 15);
                        if (level > 4)
                            level = 0;
                        delay(45);
                        setup_level();
                    } else {
                        playNoteDelay(1, 43, 15);
                        playNoteDelay(2, 43, 15);
                    }
                } else if (_next != 0x00) {
                    crashed = true;
                }

                // update buffer pointers
                if (buffer_head < BUFFSIZE) {
                    buffer_head = buffer_head + 2;
                } else {
                    buffer_head = 0;
                }
                circularBuffer[buffer_head] = head.x;
                circularBuffer[buffer_head + 1] = head.y;
                /* delete the tail if we are not growing otherwise reduce the counter of how much to grow by.*/
                if(segments == 0) {
                    if(buffer_tail < BUFFSIZE) {
                        buffer_tail = buffer_tail + 2;
                    } else {
                        buffer_tail = 0;
                    }
                } else {
                    segments --;
                }
                //draw the head
                vdp_setCharAtLocationBuf(head.x, head.y, head.pattern);
                //clear the tail
                vdp_setCharAtLocationBuf(circularBuffer[buffer_tail], circularBuffer[buffer_tail + 1], 0x00);
                nt_handleNote();
            } //End of if gamespeed % 0

            /* Refresh the display if we have not crashed.*/
            if (!crashed) {
                vdp_waitVDPReadyInt();
                vdp_refreshViewPort();
                ticks ++; //Tick the game.
            }
        } //end if not paused
    } //end while not crashed

    /* Testing with playNoteDelay instead of direct IO access.*/
    // playNoteDelay(1, 0, 15);
    // playNoteDelay(2, 0, 15);

    /* play crash sound - using NABU-LIB */
    ayWrite(6,  0x0f); //Noise Period
    ayWrite(7,  0b01000111); //mixer 01000111 = DISABLE IO B, DISABLE TONE, B, C
    ayWrite(8,  0x10); //amplitude controlled by envelope
    ayWrite(9,  0x10); //amplitude controlled by envelope
    ayWrite(10, 0x10); //amplitude controlled by envelope
    ayWrite(11, 0xa0); //Envelope period fine
    ayWrite(12, 0x40); //Envelope period course
    ayWrite(13, 0x00); //Envelope shape


    /* We have crashed - update and save high score */
    if (high_score < score ) {
        high_score = score;
    }
    /* NOTE XXX
    * If I try to save the high score here (after playing the crash sound) The game crashes.
    * It does not crash when I try to save the high score immediately before these
    * ayWrite() calls.
    * Which is a pain, because there is this short delay before the crash audio when the high
    * score is saved to disk.
    */

    /* Wait a few seconds for the player to see the carnage.*/
    delay(180);
    nt_stopSounds(); // We need to stop the sounds here.  Sometimes, if we don't, the last sound played is played for ever.
}

void main(void) {
    init();
    high_score = 100;
    /* While the user continues at the menu, play the game.*/
    while(menu()) {
        setup_game();
        game();
    }
    vdp_disableVDPReadyInt();
    #if BIN_TYPE == BIN_HOMEBREW
    __asm
        di
        rst 0x00
    __endasm;
    #else
    __asm
        jp 0
    __endasm;
    #endif
}
