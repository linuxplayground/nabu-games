// #define BIN_TYPE BIN_CPM <---- DEFINED IN MAKEFILE
#define DISABLE_HCCA_RX_INT
#define DISABLE_CURSOR

#include "NABU-LIB.h"
#include "invaders.h"
#include "string.h"
#include "nabu-games.h"
#include "audio.h"


/* Get and Set High Score is broken */
uint16_t getHighScore(void) {
    uint16_t hs;
    hs = 0;
    return hs;
}

/* Get and Set High Score is broken */
void setHighScore(uint16_t hs) __z88dk_fastcall {
    (void)hs;
}

void load_pattern_colours(uint8_t start, uint8_t count, uint8_t color) {
    vdp_setWriteAddress(_vdpColorTableAddr + start * 8);
    for (uint8_t i=0; i<count*8; i++) {
        IO_VDPDATA = color;
    }
}

void initDisplay(void) {
    initNABULib();
    vdp_clearVRAM();
    vdp_initG2Mode(1, true, false, false, false); //uint8_t bgColor, bool bigSprites, bool scaleSprites, bool autoScroll, don't split thirds

    vdp_enableVDPReadyInt();

    vdp_loadPatternTable(patterns, 0x3C8);

    //Load same colour into every colour table cell.
    vdp_setPatternColor(0xe1);

    vdp_setBackDropColor(VDP_DARK_YELLOW);

    // load sprite patterns
    vdp_loadSpritePatternNameTable(6, sprites);

    // Set up the alien colours
    load_pattern_colours(0x60, 8, 0x61);
    load_pattern_colours(0x68, 8, 0x51);
    load_pattern_colours(0x70, 8, 0x31);

}

void draw_sheilds(void) {
    for (uint8_t i=0; i<21; i++) {
        vdp_setCharAtLocationBuf(i+5, 20, shield_layout[0][i]);
        vdp_setCharAtLocationBuf(i+5, 21, shield_layout[1][i]);
    }
    for (uint8_t i=0; i<32; i++) {
        vdp_setCharAtLocationBuf(i,1,0x13);
    }
    printAtLocationBuf(0, 0, "SCORE:");
    sprintf(tb, "%06d", score);
    printAtLocationBuf(7, 0, tb);

    printAtLocationBuf(14, 0, "HIGH:");
    sprintf(tb, "%06d", high_score);
    printAtLocationBuf(20, 0, tb);
}

void display_lives(void) {
    printAtLocationBuf(27,0,"     ");
    for (uint8_t i=lives; i>0; i--) {
        vdp_setCharAtLocationBuf(32-i, 0, 0x78);
    }
}

void draw_aliens(void) {
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

void drop_aliens(void) {
    for (uint8_t i=0; i<num_rows; i++) {
        for(uint8_t j=0; j<32; j++) {
            vdp_setCharAtLocationBuf(j, top_row + i*2, 0);
        }
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

bool bullet_hit_detection(void) {
    bullet_t_x = bulletx / 8;
    bullet_t_y = bullety / 8;
    uint8_t apn =  _vdp_textBuffer[bullet_t_y * 32 + bullet_t_x]; //Alien pattern name
    if (apn == 0) {
        return false;
    }
    if (apn > 0x5F && apn < 0x78) {
        uint16_t apn_offset = (apn * 8) + (bullety % 8) -4;
        uint8_t pixels = patterns[apn_offset];
        if ((pixels & set_bit_mask[bulletx % 8]) == 0) {
            pixels = patterns[apn_offset + 1];
            if ((pixels & set_bit_mask[bulletx % 8]) == 0) {
                pixels = patterns[apn_offset + 2];
                if ((pixels & set_bit_mask[bulletx % 8]) == 0) {
                    pixels = patterns[apn_offset + 3];
                    if ((pixels & set_bit_mask[bulletx % 8]) == 0) {
                        return false;
                    }
                }
            }
        }

        // set the alien lp value to 0 to kill it.
        for (uint8_t i=0; i<5; i++) {
            if (bullet_t_y == invaders[i][0].ty) {
                for (uint8_t j=0; j<11; j++) {
                    if (bullet_t_x >= invaders[i][j].tx-1 && bullet_t_x <= invaders[i][j].tx + 1) {
                        invaders[i][j].lp = 0;
                        vdp_waitVDPReadyInt();
                        vdp_refreshViewPort(); // Remove the thing from view before displaying the explosion sprite.
                        max_invaders --;
                        uint8_t tc = 0;
                        if (apn < 0x68) {
                            tc = 6;
                            score = score + 5;
                        }
                        else if (apn < 0x70) {
                            tc = 5;
                            score = score + 25;
                        }
                        else {
                            tc = 3;
                            score = score + 45;
                        }
                        if(!extra_life_given && score >= 1500) {
                            lives ++;
                            extra_life_given = true;
                            display_lives();
                        }
                        vdp_spriteInit(EXPLODE, EXPLODE_SPRITE, (invaders[i][j].px * 2) - 4 , bullety-8, tc);
                        explode_active = EXPLODE_FRAMES;
                        //boom
                        if(!ufo_active) {
                            ayWrite(AY_CHC_AMPL, 0x10);
                            ayWrite(AY_NOISE_GEN, 0x1F);
                            ayWrite(AY_ENV_PERIOD_L, 0x00);
                            ayWrite(AY_ENV_PERIOD_H, 0x0F);
                            ayWrite(AY_ENV_SHAPE, AY_ENV_SHAPE_FADE_OUT);
                        }

                        game_speed = 2 + (max_invaders/8);
                        return true;
                    }
                }
            }
        }
    } else {
        // BRUTALLY NUKE THE SHIELD.
        if (apn < 0x0D) {
            vdp_setCharAtLocationBuf(bullet_t_x, bullet_t_y, apn+6);
            return true;
        } else if (apn < 0x13) {
            vdp_setCharAtLocationBuf(bullet_t_x, bullet_t_y, 0);
            return true;
        }
    }
    return false;
}

bool bomb_hit_detection(void) {
    bomb_t_x = bombx / 8;
    bomb_t_y = bomby / 8;
    uint8_t apn =  _vdp_textBuffer[bomb_t_y * 32 + bomb_t_x];
    if (apn == 0) {
        return false;
    } else {
        // BRUTALLY NUKE THE SHIELD.
        if (apn < 0x0D) {
            vdp_setCharAtLocationBuf(bomb_t_x, bomb_t_y, apn+6);
            return true;
        } else if (apn < 0x13) {
            vdp_setCharAtLocationBuf(bomb_t_x, bomb_t_y, 0);
            return true;
        }
    }
    return false;
}

// Find an invader in the bottom row that will be our bomber.
void select_bombing_invader(void) {
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

void new_game(void) {

    vdp_waitVDPReadyInt();
    uint8_t tmp = IO_VDPLATCH;  //dummy read
    vdp_clearScreen();

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
    if( !level_up) {
        score = 0;
        // godMode = false;
        level = 0;
        lives = 3;
        display_lives();
    } else {
        level ++;
        for(uint8_t z = 0; z<level; z++) {
            drop_aliens();
        }
    }
    alien_note_index = 0;
    beat_counter = 4;
    fire_count_index = 22;
    ufo_active = false;
}

// Play a game.
void game(void) {

    ayWrite(AY_ENABLES, 0b01011100); //Only noise on C and Tones on A

    while (running) {
        ticks ++;
        // have we won?
        if (max_invaders == 0) {
            level_up = true;
            running = false;
        }

        // Check player input.
        if (isKeyPressed()) {
            // if (getChar() == 0x20) {
            //     godMode = true;
            // } else {
                running = false;
            // }
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
                bullety = 177;
                vdp_spriteInit(BULLET, BULLET_SPRITE, bulletx, bullety, 5);
                bullet_active = true;
                // pew
                if(!ufo_active) {
                    playNoteDelay(0,60,8);
                }

                fire_count_index --;
                if (fire_count_index == 0) {
                    fire_count_index = 15;
                    // we launch a UFO every 16 bullets fired.
                    if(first_bomb_delay == 0) {
                        ufo_x = 240;
                        vdp_spriteInit(UFO, UFO_SPRITE, 8, ufo_x, 6);
                        ufo_active = true;
                        // Turn on UFO siren
                        ayWrite(AY_CHB_AMPL, 0x1F);
                        ayWrite(AY_NOISE_GEN, 0x00);
                        ayWrite(AY_CHB_TONE_H, 0x00);
                        ayWrite(AY_CHB_TONE_L, 0x20);
                        ayWrite(AY_ENV_PERIOD_L, 0xff);
                        ayWrite(AY_ENV_PERIOD_H, 0x02);
                        ayWrite(AY_ENV_SHAPE, AY_ENV_SHAPE_TRIANGLE);
                    }
                }
            }
        }

        // Update screen
        if (ticks % game_speed == 0) {
            if (drop_flag) {
                drop_aliens();
                num_rows = ((bottom_row - top_row) / 2) + 1;
            }
            draw_aliens();
            if (!ufo_active) {
                // We only play the march when the UFO is not there.
                beat_counter --;
                if (beat_counter == 0 ) {
                    alien_note_index ++;
                    if (alien_note_index > 3)
                        alien_note_index = 0;
                    ayWrite(AY_CHB_AMPL, 0x0F);
                    playNoteDelay(1, alien_notes[alien_note_index], 6);
                    beat_counter = 4;
                }
            }
            ticks = 0;
        }

        // Bullet logic
        if (bullet_active) {
            if (ufo_active) {
                if ((vdpStatusRegVal & 0b00100000)>0 && bullety < 24) {
                    score = score + ufo_scores[fire_count_index];
                    if(!extra_life_given && score >= 1500) {
                        lives ++;
                        extra_life_given = true;
                        display_lives();
                    }
                    ufo_active = false;
                    vdp_disableSprite(UFO);
                    vdp_spriteInit(EXPLODE, EXPLODE_SPRITE, ufo_x+8 , bullety-4, 6);
                    explode_active = EXPLODE_FRAMES;
                    //boom
                    ayWrite(AY_CHC_AMPL, 0x10);
                    ayWrite(AY_NOISE_GEN, 0x1F);
                    ayWrite(AY_ENV_PERIOD_L, 0x00);
                    ayWrite(AY_ENV_PERIOD_H, 0x0F);
                    ayWrite(AY_ENV_SHAPE, AY_ENV_SHAPE_FADE_OUT);

                    vdp_disableSprite(BULLET);
                    bullet_active = false;

                    sprintf(tb, "%06d", score);
                    printAtLocationBuf(7, 0, tb);
                    if (score > high_score) {
                        high_score = score;
                        sprintf(tb, "%06d", high_score);
                        printAtLocationBuf(20, 0, tb);
                    }
                }
            }
            if (bullety > 8) {
                if (bullet_hit_detection()) {
                    vdp_disableSprite(BULLET);
                    bullet_active = false;

                    sprintf(tb, "%06d", score);
                    printAtLocationBuf(7, 0, tb);
                    if (score > high_score) {
                        high_score = score;
                        sprintf(tb, "%06d", high_score);
                        printAtLocationBuf(20, 0, tb);
                    }

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
            //if ((vdpStatusRegVal & 0b00100000)>0 && !godMode) {
            if ((vdpStatusRegVal & 0b00100000)>0) {
                if (bomby > 176) {
                    if( bombx > playerx && bombx < playerx + 16 ) {
                        vdp_disableSprite(PLAYER);
                        vdp_disableSprite(UFO);
                        vdp_spriteInit(EXPLODE, EXPLODE_SPRITE, playerx, 176, 15);
                        // From snake. - Disaster!
                        ayWrite(AY_NOISE_GEN,  0x0f); //Noise Period
                        ayWrite(AY_ENABLES,  0b01000111); //mixer 01000111 = DISABLE IO B, DISABLE TONE, B, C
                        ayWrite(AY_CHA_AMPL,  0x10); //amplitude controlled by envelope
                        ayWrite(AY_CHB_AMPL,  0x10); //amplitude controlled by envelope
                        ayWrite(AY_CHC_AMPL, 0x10); //amplitude controlled by envelope
                        ayWrite(AY_ENV_PERIOD_L, 0xa0); //Envelope period fine
                        ayWrite(AY_ENV_PERIOD_H, 0x40); //Envelope period course
                        ayWrite(AY_ENV_SHAPE, 0x00); //Envelope shape

                        // Deal with new life.
                        lives --;
                        if (lives < 1) {
                            running = false;
                            level_up = false;
                        } else {

                            display_lives();
                            delay(60);   // Allow explosion and crash sound to end
                            vdp_disableSprite(EXPLODE);
                            vdp_disableSprite(BOMB);
                            bomb_active = false;
                            vdp_disableSprite(BULLET);
                            bullet_active = false;
                            vdp_disableSprite(UFO);
                            ufo_active = false;

                            // We need to recenter the player and blink it 5 times.
                            playerx = 120;
                            vdp_spriteInit(PLAYER, PLAYER_SPRITE, 120, 176, 15);
                            for(uint8_t i=0; i<5; i++) {
                                vdp_disableSprite(PLAYER);
                                delay(15);
                                vdp_spriteInit(PLAYER, PLAYER_SPRITE, 120, 176, 15);
                                delay(15);
                            }
                            ayWrite(AY_ENABLES, 0b01011100); //Only noise on C and Tones on A
                        }
                    }
                }
            }
        } else {
            if(first_bomb_delay == 0) {
                // select an invader to drop the bomb
                select_bombing_invader(); //Sets bombx and bomby - because global variables for the win!
                vdp_spriteInit(BOMB, BOMB_SPRITE, bombx, bomby, 11);
                bomb_active = true;
            } else {
                first_bomb_delay --;
            }
        }

        // UFO Logic
        if (ufo_active) {
            ufo_x --;
            vdp_setSpritePosition(UFO, ufo_x, 8);
            if (ufo_x == 0) {
                vdp_disableSprite(UFO);
                ufo_active = false;
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
    delay(60);
}

// Display a menu
bool menu(void) {
    ayWrite(AY_ENABLES, 0b01111111); // disable all sounds
    vdp_waitVDPReadyInt();
    uint8_t tmp = IO_VDPLATCH;  //dummy read
    vdp_clearScreen();

    if (level_up == false) {
        centerText("JOYSTICK ONLY",8);
        centerText("GAME OVER",10);
        centerText("BTN TO PLAY AGAIN",11);
    } else {
        centerText("BTN TO PLAY NEXT LEVEL",9);
    }

    centerText("INVADERS - V3.5",4);
    centerText("BY PRODUCTIONDAVE",5);

    centerText("SCORE:     ",13);
    sprintf(tb, "%06d", score);
    printAtLocationBuf(17, 13, tb);

    centerText("HIGH SCORE:     ",14);
    sprintf(tb, "%06d", high_score);
    printAtLocationBuf(19, 14, tb);

    centerText("ESC TO QUIT",16);

    vdp_waitVDPReadyInt();
    tmp = IO_VDPLATCH;  //dummy read
    vdp_waitVDPReadyInt();
    vdp_refreshViewPort();

    while (true) {
        if (getJoyStatus(0) & Joy_Button) {
            return true;
        }
        if (isKeyPressed()) {
            if (getChar() == 0x1b) {
                return false;
            } else {
                return true;
            }
        }
    }
}

void main(void) {
    initDisplay();
    play_again = true;
    level_up = false;
    game_speed = DEFAULT_GAME_SPEED;
    score = 0;
    high_score = 0;
    extra_life_given = false;

    while (play_again) {
        if (menu()) {
            delay(60);
            new_game();
            display_lives();
            game();
            game_speed = DEFAULT_GAME_SPEED;
        } else {
            play_again = false;
        };
        //setHighScore(high_score);
    }

    vdp_disableVDPReadyInt();
    initNABULIBAudio(); //Make sure to reset audio.

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
