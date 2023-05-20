// #define BIN_TYPE BIN_CPM <---- DEFINED IN MAKEFILE
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "invaders.h"

uint8_t running = 1;
int8_t dir = 1;
int8_t new_dir = 1;
uint8_t ticks = 0;
uint8_t drop_flag = 0;
uint8_t top_row = 2;
uint8_t bottom_row = 10;
uint8_t num_rows = 5;
// uint8_t num_cols = 11;
uint8_t playerx = 120;
uint8_t update_player = false;
uint8_t bulletx = 0;
uint8_t bullety = 176;
uint8_t bullet_active = false;
uint8_t bullet_t_x, bullet_t_y = 0;

// game settings
uint8_t game_speed = 8;
uint8_t player_speed = 2;
uint8_t bullet_speed = 3;

void initDisplay() {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, true, false, false, false); //uint8_t bgColor, bool bigSprites, bool scaleSprites, bool autoScroll, don't split thirds
    vdp_enableVDPReadyInt();
    vdp_loadPatternTable(patterns,392);
    //Load same colour into every colour table cell.
    vdp_setPatternColor(0xe1);
    //initG2Mode loads the fb with spaces.  (0x20) We want 0x00.
    uint8_t *start = _vdp_textBuffer;
    uint8_t *end = start + (32 * 24);
    do {
        *start = 0x00;
        start++;
    } while (start != end);

    // load sprite patterns
    vdp_loadSpritePatternNameTable(5, sprites);
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
        for (uint8_t j=0; j<16; j++) {

            // figure out new x tile location.
            invaders[i][j].px += dir;

            tx = invaders[i][j].px / 4;
            invaders[i][j].tx = tx;
            ty = invaders[i][j].ty;

            if (invaders[i][j].lp != 0) {
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
    }
    dir = new_dir;
}

void drop_aliens() {
    for (uint8_t i=0; i<5; i++) {
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
                // if(invaders[i][j].tx > temp_x) {
                //     temp_x = invaders[i][j].tx;
                //     num_cols ++;
                // }
            }   
        }
    }
    top_row ++;
    drop_flag = 0;
}

bool bullet_hit_detection() {
    bullet_t_x = bulletx / 8;
    bullet_t_y = bullety / 8;
    uint8_t apn =  _vdp_textBuffer[bullet_t_y * 32 + bullet_t_x]; //Alien patter name
    if (apn > 0 && apn < 25) {
        // Is tile level collision detection good enough?
        for (uint8_t i=0; i<5; i++) {
            if (bullet_t_y == invaders[i][0].ty) {
                for (uint8_t j=0; j<11; j++) {
                    if (bullet_t_x == invaders[i][j].tx || bullet_t_x == invaders[i][j].tx+1) {
                        invaders[i][j].lp = 0;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void main() {
    initDisplay();

    draw_sheilds();

    // initialise the player sprite/
    vdp_spriteInit(PLAYER, PLAYER_SPRITE, 120, 176, 15);


    while (running) {
        ticks ++;
        // Check player input.
        if (isKeyPressed()) {
            running = false;
        }
        // Left
        if (getJoyStatus(0) & Joy_Left) {
            playerx = playerx - player_speed;
            if (playerx < 8) {
                playerx = 8;
            }
            update_player = true;
        }
        // Right
        if (getJoyStatus(0) & Joy_Right) {
            playerx = playerx + player_speed;
            if (playerx > 240) {
                playerx = 240;
            }
            update_player = true;
        }
        // Shoot
        if (getJoyStatus(0) & Joy_Button) {
            if (!bullet_active) {
                bulletx = playerx + 8;
                bullety = 176;
                vdp_spriteInit(BULLET, BULLET_SPRITE, bulletx, bullety, 5);
                bullet_active = true;
            }
        }
        // Update screen
        if (ticks % game_speed == 0) {
            num_rows = (bottom_row - top_row) / 2 + 1;
            if (drop_flag) {
                drop_aliens();
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
                    bullety = bullety - bullet_speed;
                }
            } else {
                vdp_disableSprite(BULLET);
                bullet_active = false;
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
        if (ticks == 0) {
            vdp_waitVDPReadyInt();  // kill a frame.
            vdp_refreshViewPort();
        }
    }

}
