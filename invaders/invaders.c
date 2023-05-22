// #define BIN_TYPE BIN_CPM <---- DEFINED IN MAKEFILE
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "invaders.h"
#include "string.h"

// game settings
#define GAME_SPEED 8
#define PLAYER_SPEED 2
#define BULLET_SPEED 3
#define BOMB_SPEED  3
#define EXPLODE_FRAMES 10


void load_pattern_colours(uint8_t start, uint8_t count, uint8_t color) {
    vdp_setWriteAddress(_vdpColorTableAddr + start * 8);
    for (uint8_t i=0; i<count*8; i++) {
        IO_VDPDATA = color;
    }
}

void clear_screen() {
    //initG2Mode loads the fb with spaces.  (0x20) We want 0x00.
    uint8_t *start = _vdp_textBuffer;
    uint8_t *end = start + (32 * 24);
    do {
        *start = 0x00;
        start++;
    } while (start != end);
}

void initDisplay() {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, true, false, false, false); //uint8_t bgColor, bool bigSprites, bool scaleSprites, bool autoScroll, don't split thirds
    vdp_enableVDPReadyInt();

    //Load same colour into every colour table cell.
    vdp_setPatternColor(0xe1);
    vdp_setBackDropColor(VDP_DARK_YELLOW);

    clear_screen();

    // load sprite patterns
    vdp_loadSpritePatternNameTable(5, sprites);

    // Set up the alien colours
    load_pattern_colours(1, 8, 0x61);
    load_pattern_colours(9, 8, 0x51);
    load_pattern_colours(17, 8, 0x31);
}

void draw_sheilds() {
    for (uint8_t i=0; i<21; i++) {
        vdp_setCharAtLocationBuf(i+5, 20, shield_layout[0][i]);
        vdp_setCharAtLocationBuf(i+5, 21, shield_layout[1][i]);
    }
}

void draw_aliens() {
    uint8_t tx = 0;
    uint8_t ty = 0;
    uint8_t tp = 0;

    for (uint8_t i=0; i<num_rows; i++) {
        vdp_setCharAtLocationBuf(invaders[i][0].tx, invaders[i][0].ty, 0);
        for (uint8_t j=0; j<11; j++) {

            // figure out new x tile location.
            invaders[i][j].px += dir;

            tx = invaders[i][j].px / 4;
            invaders[i][j].tx = tx;
            ty = invaders[i][j].ty;

            if (invaders[i][j].lp > 0) {
                tp = invaders[i][j].lp + ((invaders[i][j].px & 0x03) * 2);
                vdp_setCharAtLocationBuf(tx, ty, tp);
                vdp_setCharAtLocationBuf(tx + 1, ty, tp + 1);
                if (tx >= 30) {
                    new_dir = -1;
                    drop_flag = 1;
                }
                
                if (tx < 1) {
                    new_dir = 1;
                    drop_flag = 1;
                }
            } else {
                vdp_setCharAtLocationBuf(tx, ty, 0);
                vdp_setCharAtLocationBuf(tx + 1, ty, 0);
            }
        }
        vdp_setCharAtLocationBuf(tx+2, ty, 0);
    }
    dir = new_dir;
}

void drop_aliens() {
    for (uint8_t i=0; i<num_rows; i++) {
        for(uint8_t j=0; j<32; j++) {
            vdp_setCharAtLocationBuf(j, top_row + i*2, 0);
        }
        // uint8_t temp_x = 0;
        // num_cols = 0;
        for (uint8_t j=0; j<11; j++) {
            invaders[i][j].ty ++;
            if (invaders[i][j].ty > 22) {
                running = false;
            }
            // We need to know which is the bottom row.  We can use this when
            // drawing the aliens so we don't delete the shields too early.

            if (invaders[i][j].lp != 0) {
                bottom_row = invaders[i][j].ty;
            }   
        }
    }
    top_row ++;
    drop_flag = 0;
}

bool bullet_hit_detection() {
    bullet_t_x = bulletx / 8;
    bullet_t_y = bullety / 8;
    uint8_t apn =  _vdp_textBuffer[bullet_t_y * 32 + bullet_t_x]; //Alien pattern name
    if (apn == 0) {
        return false;
    }
    if (apn < 25) {

        uint8_t pixels = patterns[(8 * apn) + (bullety % 8)];
        if ((pixels & set_bit_mask[bulletx % 8]) == 0) {
            return false;
        }

        // set the alien lp value to 0 to kill it.
        for (uint8_t i=0; i<5; i++) {
            if (bullet_t_y == invaders[i][0].ty) {
                for (uint8_t j=0; j<11; j++) {
                    if (bullet_t_x == invaders[i][j].tx || bullet_t_x == invaders[i][j].tx+1) {
                        invaders[i][j].lp = 0;
                        vdp_waitVDPReadyInt();
                        vdp_refreshViewPort(); // Remove the thing from view before displaying the explosion sprite.
                        max_invaders --;
                        uint8_t tc = 0;
                        if (apn < 9) tc = 6;
                        else if (apn < 17) tc = 5;
                        else tc = 3;
                        vdp_spriteInit(EXPLODE, EXPLODE_SPRITE, (invaders[i][j].px * 2) - 4 , bullety-8, tc);
                        explode_active = EXPLODE_FRAMES;
                        return true;
                    }
                }
            }
        }
    } else {
        // BRUTALLY NUKE THE SHIELD.
        if (apn < 31)
            vdp_setCharAtLocationBuf(bullet_t_x, bullet_t_y, apn+6);
        else
            vdp_setCharAtLocationBuf(bullet_t_x, bullet_t_y, 0);
        return true;
    }
    return false;
}

bool bomb_hit_detection() {
    bomb_t_x = bombx / 8;
    bomb_t_y = bomby / 8;
    uint8_t apn =  _vdp_textBuffer[bomb_t_y * 32 + bomb_t_x];
    if (apn == 0) {
        return false;
    } else {
        if (apn > 36) {
            vdp_setCharAtLocationBuf(bomb_t_x, bomb_t_y, 0);
            return true;
        } else if (apn > 24) {
            // BRUTALLY NUKE THE SHIELD. (well - it does so in stages.)
            vdp_setCharAtLocationBuf(bomb_t_x, bomb_t_y, apn+6);
            return true;
        }
    }
    return false;
}

// Find an invader in the bottom row that will be our bomber.
void select_bombing_invader() {
    uint8_t selected = false;
    uint8_t x = 0;
    uint8_t y = 0;
    do {
        x = rand() % 11;
        y = rand() % 5;
        if (invaders[y][x].lp != 0) {
            selected = true;
            bombx = invaders[y][x].tx * 8 + (invaders[y][x].px % 4) + 6;
            bomby = invaders[y][x].ty * 8;
        }
    } while (!selected);
}

void new_game() {
    clear_screen();
    vdp_loadPatternTable(patterns,344); //Aliens and shields
    draw_sheilds();

    // initialise the player sprite/
    vdp_spriteInit(PLAYER, PLAYER_SPRITE, 120, 176, 15);
    // initialise default invader positions
    memcpy(invaders, invaders_default, sizeof(invaders));
    vdp_disableSprite(BOMB);
    vdp_disableSprite(BULLET);
    vdp_disableSprite(EXPLODE);
    running = 1;
    dir = 1;
    new_dir = 1;
    ticks = 0;
    max_invaders = 55;
    drop_flag = 0;
    top_row = 2;
    bottom_row = 10;
    num_rows = 5;
    playerx = 120;
    update_player = false;
    bulletx = 0;
    bullety = 176;
    bullet_active = false;
    bullet_t_x, bullet_t_y = 0;
    first_bomb_delay = 120;  //2 seconds before first bomb.
    bombx = 0;
    bomby = 0;
    bomb_active = false;
    bomb_t_x, bomb_t_y = 0;
    explode_active = 0;
}

// Play a game.
void game() {
    while (running) {
        ticks ++;
        // have we won?
        if (max_invaders == 0) {
            running = false;
        }

        // Check player input.
        if (isKeyPressed()) {
            running = false;
        }
        // Left
        if (getJoyStatus(0) & Joy_Left) {
            playerx = playerx - PLAYER_SPEED;
            if (playerx < 8) {
                playerx = 8;
            }
            update_player = true;
        }
        // Right
        if (getJoyStatus(0) & Joy_Right) {
            playerx = playerx + PLAYER_SPEED;
            if (playerx > 240) {
                playerx = 240;
            }
            update_player = true;
        }
        // Shoot
        if (getJoyStatus(0) & Joy_Button) {
            if (!bullet_active) {
                bulletx = playerx + 8;
                bullety = 22*8;
                vdp_spriteInit(BULLET, BULLET_SPRITE, bulletx, bullety, 5);
                bullet_active = true;
            }
        }
        // Update screen
        if (ticks % GAME_SPEED == 0) {
            if (drop_flag) {
                drop_aliens();
                num_rows = ((bottom_row - top_row) / 2) + 1;
            }
            draw_aliens();
            ticks = 0;
        }

        // Bullet logic
        if (bullet_active) {
            if (bullety > 4) {
                if (bullet_hit_detection()) {
                    vdp_disableSprite(BULLET);
                    bullet_active = false;
                } else {
                    bullety = bullety - BULLET_SPEED;
                }
            } else {
                vdp_disableSprite(BULLET);
                bullet_active = false;
            }
        }

        // Bomb logic
        if (bomb_active) {
            if(bomby < 188) {
                if (bomb_hit_detection()) {
                    vdp_disableSprite(BOMB);
                    bomb_active = false;
                } else {
                    bomby = bomby + BOMB_SPEED;
                }
            } else {
                vdp_disableSprite(BOMB);
                bomb_active = false;
            }
            if ((vdpStatusRegVal & 0b00100000)>0) {
                if (bomby > 176) {
                    if( bombx > playerx && bombx < playerx + 16 ) {
                        vdp_disableSprite(PLAYER);
                        vdp_spriteInit(EXPLODE, EXPLODE_SPRITE, playerx, 176, 15);
                        running = false;
                    }
                }
            }
        } else {
            if(first_bomb_delay == 0) {
                // select an invader to drop the bomb
                select_bombing_invader(); //Sets bombx and bomby - because global variables for the win!
                vdp_spriteInit(BOMB, BOMB_SPRITE, bombx, bomby, 6);
                bomb_active = true;
            } else {
                first_bomb_delay --;
            }

        }

        vdp_waitVDPReadyInt();
        // Update player and bullet sprites
         if (update_player) {
            vdp_setSpritePosition(PLAYER, playerx, 175);
            update_player = false;
        }
        if (bullet_active) {
            vdp_setSpritePosition(BULLET, bulletx, bullety);
        }
        if (bomb_active) {
            vdp_setSpritePosition(BOMB, bombx, bomby);
        }
        if (explode_active > 0) {
            explode_active --;
            if (explode_active == 0) {
                vdp_disableSprite(EXPLODE);
                explode_active = EXPLODE_FRAMES;
            }
        }
        if (ticks == 0) {
            vdp_waitVDPReadyInt();  // kill a frame.
            vdp_refreshViewPort();
        }
    }
}

// Display a menu
bool menu() {

    uint8_t k = getChar();
    if(k == 0x1b) {
        return false;
    } else {
        return true;
    }
}

void main() {
    initDisplay();
    bool play_again = true;
    while (play_again) {
        if (menu()) {
            new_game();
            running = true;
            game();
        } else {
            play_again = false;
        };
    }
}
