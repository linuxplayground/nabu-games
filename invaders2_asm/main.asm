
is_nabu:        equ 1
game_speed:     equ 12
player_speed:   equ 2
bullet_speed:   equ 4

        include 'macros.asm'


        org     0x100

        ld      sp,.stack

if is_nabu = 1
; set up kbd isr
        im      2
        ld      hl, isrKeyboard
        ld      (0xff00+4), hl
endif

main:
        call    init_game

gameloop:
        ld      a,(ticks)
        cp      game_speed
        jr      c,.player_controls
        xor     a
        ld      (ticks),a
        call    draw_invaders
        ld      a,(drop_invaders_flag)
        or      a
        jr      z,.player_controls
        call    drop_invaders

.player_controls:
        call    isKeyPressed
        or      a
        jr      nz,.exit
        call    getJoyStatus
        ld      c,a                     ; save status for now.

        and     joy_map_left
        jp      z,.not_left
        call    move_player_left
        jp      .check_button
.not_left:
        ld      a,c
        and     joy_map_right
        jp      z,.not_right
        call    move_player_right
.check_button:
.not_right:     ; SHOOT
        ld      a,(bullet_active)
        or      a
        jr      nz,.move_player
        ld      a,c
        and     joy_map_button
        jp      z,.move_player
        ld      a,(PLAYER_ATTRIBUTES)
        ld      (BULLET_ATTRIBUTES),a           ; sets Y of bullet to player Y
        ld      a,(PLAYER_ATTRIBUTES + 1)
        add     8
        ld      (BULLET_ATTRIBUTES + 1),a       ; sets X of bullet to player X
        ld      a,1
        ld      (bullet_active),a

.move_player:
        call    set_player_attributes
        call    animate_bullet

.flush:
        meminc  ticks
        call    tms_flush_buffer
        jp      gameloop

.exit:        
        call    cpm_terminate

init_game:
; initialization - setup vdp etc.
        call    tms_clear_vram
        call    tms_init_g2
        call    tms_clear_screen

        ld      b,tms_white<<4|tms_black
        call    tms_set_all_colors              ; sets all color table entries to the same value.
        call    create_invader_pattern_table
        call    create_invader_color_table

; set up shield patterns
        call    create_shield_pattern_table
        call    draw_shield_layout
        
; set up player
        call    create_player_sprite
        call    set_player_attributes

; set up player bullet
        call    create_bullet_sprite
        call    set_bullet_attributes
        ret

include 'utils.asm'
include 'tms.asm'
include 'patterns.asm'
include 'invaders.asm'
include 'shield.asm'
include 'player.asm'

; game variables
ticks:          db 0
x_dir:          db 1
new_x_dir:      db 1
drop_invaders_flag:  db 0

        ds      1024
.stack: equ     $
