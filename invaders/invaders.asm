; space invaders clone
; to work on tms9918a graphics and designed for the nabu and Z80-Retro! and run
; on CP/M

is_nabu:        equ 1

        include 'macros.asm'

        org     0x100

if is_nabu = 1
; set up kbd isr
        im      2
        ld      hl, isrKeyboard
        ld      (0xff00+4), hl
endif

; initialization - setup vdp etc.
        call    tms_clear_vram
        call    tms_init_g2
        call    tms_clear_screen

        ld      b,tms_white<<4|tms_black
        call    tms_set_all_colors              ; sets all color table entries to the same value.
; set up alien patterns and colours
        call    create_game_field
        call    create_invader_pattern_table
        call    create_invader_color_table

; set up shield patterns
        call    create_shield_pattern_table
        
; set up player
        call    create_player_sprite
        call    set_player_attributes
; set up player bullet
        call    create_bullet_sprite
        call    set_bullet_attributes


include 'utils.asm'
include 'tms.asm'
include 'patterns.asm'
include 'aliens.asm'
include 'shield.asm'
include 'player.asm'
include 'tile_functions.asm'