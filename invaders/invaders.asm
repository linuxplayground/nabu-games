; space invaders clone
; to work on tms9918a graphics and designed for the nabu.
        include 'macros.asm'

invader_1_pattern_start:  equ     8          ; the first pattern should ramain blank.
invader_2_pattern_start:  equ     invader_1_pattern_start + 16  ; second pattern starts 16 bytes later
invader_3_pattern_start:  equ     invader_2_pattern_start + 16  ; third pattern 16 bytes after that.

; macros
tmsdisalbeoutput: macro
        ld      d, 0xa0
        ld      b, 0x01
        call    tms_set_register
endm

tmsenableoutput: macro
        ld      d, 0xe0
        ld      b, 0x01
        call    tms_set_register
endm


; _main (i know c style - but it's useful to think of this as the main entry point)
        org     0x100

; initialization - setup vdp etc.
        call    tms_clear_vram
        call    tms_init_g2
        call    tms_clear_screen

        ; ld      b,tms_dark_yellow
        ; call    tms_set_backdrop_color

        ld      b,tms_white<<4|tms_black
        call    tms_set_all_colors              ; sets all color table entries to the same value.
        call    create_invader_pattern_table
        
; init game
; set up direction
        ld      a,1
        ld      (x_dir),a
        ld      (game_x_offset),a

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
        cp      20                              ; alien speed - low = fast
        jp      nc, do_update_game_field
        ld      (ticks),a
        jp      end_loop
do_update_game_field:
        ld      a,(x_dir)
        ld      b,a
        ld      a,(game_x_offset)
        add     a,b
        cp      32
        jp      nz,flush
reset_frame:
        xor     a
        ld      (game_x_offset),a
flush:
        ld      (game_x_offset),a
        xor     a
        ld      (ticks),a
        ; call    hexdump_a
        fillmem game_field_buffer_start 0x00 game_field_buffer_size       ; start 2 rows down, fill 10 rows
        call    update_game_field
        call    tms_flush_buffer

end_loop:
        call    tms_wait
        jp      gameloop

frame:                  db 0
ticks:                  db 0
x_dir:                  db 0
game_field_offset:      db 0
game_x_offset:          db 0


include 'tms.asm'
include 'utils.asm'
include 'patterns.asm'
include 'aliens.asm'
