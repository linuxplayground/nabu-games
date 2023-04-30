; space invaders clone
; to work on tms9918a graphics and designed for the nabu.
        include 'macros.asm'


; _main (i know c style - but it's useful to think of this as the main entry point)
        org     0x100

; initialization - setup vdp etc.
        call    tms_clear_vram
        call    tms_init_g2
        call    tms_clear_screen

        ; ld      b,tms_dark_yellow
        ; call    tms_set_backdrop_colors

        ld      b,tms_white<<4|tms_black
        call    tms_set_all_colors              ; sets all color table entries to the same value.
        call    create_invader_pattern_table
        call    create_invader_color_table
        
; init game
; set up direction
        ld      a,8
        ld      (game_x_offset),a
        ld      a,2
        ld      (game_y_offset),a
        ld      a,1
        ld      (x_dir),a

; set up framebuffer
        fillmem tms_buffer 0x00 0x300
        call    update_game_field
        call    tms_wait
        call    tms_flush_buffer

; game loop
gameloop:
        call    check_cpm_key
        ld      a, (ticks)
        inc     a
        cp      8                              ; alien speed - low = fast
        jp      nc, do_update_game_field
        ld      (ticks),a
        jp      end_loop
do_update_game_field:
        ld      a,(x_dir)
        or      a
        jp      z,.move_left
        ; fall through
.move_right:
        ld      a,(game_x_offset)
        inc     a
        ld      (game_x_offset),a
        cp      32
        jp      nz,flush
        ld      a,0
        ld      (x_dir),a
        ld      a,(game_y_offset)
        inc     a
        ld      (game_y_offset),a
        cp      11
        jp      p,reset_game
        jp      flush
.move_left:
        ld      a,(game_x_offset)
        dec     a
        ld      (game_x_offset),a
        cp      8
        jp      nz,flush
        ld      a,1
        ld      (x_dir),a
        ld      a,(game_y_offset)
        inc     a
        ld      (game_y_offset),a
        cp      11
        jp      p,reset_game
        ; fall through
flush:
        xor     a
        ld      (ticks),a
        ; call    hexdump_a
        fillmem game_field_buffer_start 0x00 game_field_buffer_size       ; start 2 rows down, fill 10 rows
        call    update_game_field
        call    tms_flush_buffer
end_loop:
        call    tms_wait
        jp      gameloop

reset_game:
        ld      a,2
        ld      (game_y_offset),a
        jp      gameloop

frame:                  db 0
ticks:                  db 0
x_dir:                  db 0
game_field_offset:      db 0
game_x_offset:          db 0
game_y_offset:          db 0


include 'tms.asm'
include 'utils.asm'
include 'patterns.asm'
include 'aliens.asm'
