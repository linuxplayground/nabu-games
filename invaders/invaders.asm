; space invaders clone
; to work on tms9918a graphics and designed for the nabu and Z80-Retro! and run
; on CP/M

is_nabu:        equ 1
game_speed:     equ 8
player_speed:   equ 2
bullet_speed:   equ 4

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

; gameloop
reset:
        call    draw_shield_layout
        ; set up gamefield offset to top left, 2 rows down.
        xor     a
        ld      (x_px_offset),a
        ld      (x_t_offset),a
        ld      a,2
        ld      (y_t_offset),a

gameloop:
        ; check for player input.
.check_key_press:
        call    isKeyPressed
        or      a
        jp      nz,0                    ; warm boot if a key is pressed. (DEBUG)

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
.do_tick:
        ld      a,(ticks)
        cp      game_speed
        jp      z,.animate
        inc     a
        ld      (ticks),a
        jp      .update_game_field
.animate:
        ; reset ticks before we start
        xor     a
        ld      (ticks),a
        ; jp      .flush                ;;; freeze frame
        ld      a,(x_dir)
        or      a
        jr      z,.aliens_moving_left
.aliens_moving_right:
        ld      a,(x_px_offset)
        inc     a
        ld      (x_px_offset),a
        div4
        ld      (x_t_offset),a
        ld      a,(x_px_offset)
        cp      37
        jp      c,.update_game_field
        ld      a,0
        ld      (x_dir),a
        ld      a,(y_t_offset)
        inc     a
        ld      (y_t_offset),a
        call    clear_game_field
        jp      .update_game_field
.aliens_moving_left:
        ld      a,(x_px_offset)
        dec     a
        ld      (x_px_offset),a
        div4
        ld      (x_t_offset),a
        ld      a,(x_px_offset)
        or      a
        jp      nz,.update_game_field
        ld      a,1
        ld      (x_dir),a
        ld      a,(y_t_offset)
        inc     a
        ld      (y_t_offset),a
        call    clear_game_field
        ; fall through
.update_game_field:        
        call    update_game_field
.flush:        
        ; call    tms_wait
        call    tms_flush_buffer
        jp      gameloop

; variables
ticks:          db 0

include 'utils.asm'
include 'tms.asm'
include 'patterns.asm'
include 'gamefield.asm'
include 'shield.asm'
include 'player.asm'
include 'tile_functions.asm'
