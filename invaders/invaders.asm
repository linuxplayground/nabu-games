; space invaders clone
; to work on tms9918a graphics and designed for the nabu and Z80-Retro! and run
; on CP/M

        include 'macros.asm'

        org     0x100

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

reset:
; init gamefield (aliens)
        ld      a,8
        ld      (game_x_offset),a
        ld      a,2
        ld      (game_y_offset),a
        ld      a,1
        ld      (x_dir),a
        ld      a,13
        ld      (max_rows),a
        ld      a,9
        ld      (game_rows),a

; copy initial gamefield data over to variable gamefield data for game play.
; this will allow us to later on alter the values in the gamefield array.

        fillmem tms_buffer 0x00 0x300           ; init framebuffer
        call    update_game_field               ; draw aliens in initial positions.
        call    tms_wait                        ; wait for vsync status register on vdp
        call    tms_flush_buffer                ; write framebuffer to the vdp.

        call    draw_shield_layout              ; add the shields to the buffer.
; game loop
.gameloop:
        call    check_cpm_key                   ; check for user input. Terminates at the moment.
        ld      a, (ticks)                      ; increment ticks.  We animate every 8th vsync
        inc     a
        cp      8
        jp      nc, .do_update_game_field
        ld      (ticks),a
        call    tms_wait                        ; game timing is defined by vsync
        jp      .gameloop
.do_update_game_field:                          ; we hit the 8th vsync so time to start calculating the frame buffer
        ld      a,(x_dir)                       ; get the direction of the aliens
        or      a
        jp      z,.move_left                    ; if dir=0 we are moving left.
        ; fall through
.move_right:
        ld      a,(game_x_offset)               ; increment x pixel offset
        inc     a
        ld      (game_x_offset),a
        cp      32                              ; check for right side boundary
        jp      nz,.flush
        ld      a,0
        ld      (x_dir),a                       ; if right side boundary is hit, set direction to 0.
        ld      a,(game_y_offset)               ; increment the y offiset (down one row)
        inc     a
        ld      (game_y_offset),a
        ld      hl,max_rows
        cp      (hl)                            ; check for bottom boundary
        jp      p,.reset_game                   ; if bottom boundary then reset game.
        jp      .flush
.move_left:
        ld      a,(game_x_offset)               ; decrement the x pixel offset
        dec     a
        ld      (game_x_offset),a
        cp      8                               ; check for left boundary
        jp      nz,.flush
        ld      a,1                             ; if left boundary hit, set direction to 1
        ld      (x_dir),a
        ld      a,(game_y_offset)               ; increment y offset (down one row)
        inc     a
        ld      (game_y_offset),a
        ld      hl,max_rows
        cp      (hl)                            ; if bottom boundary hit, reset game.
        jp      p,.reset_game
        ; fall through
.flush:
        xor     a
        ld      (ticks),a                       ; reset tick count to 0.
        call    update_game_field               ; draw the current game field on to the buffer.
.end_loop:
        call    tms_wait                        ; wait for vsync
        call    tms_flush_buffer                ; fast flush the buffer to to the vdp.
        jp      .gameloop


; Move all the aliens back to their starting posistions.
.reset_game:
        jp      reset

; initialize variables
ticks:                  db 0
x_dir:                  db 1
game_field_offset:      db 0
game_x_offset:          db 8
game_y_offset:          db 2
game_rows:              db 9
max_rows:               db 13

include 'tms.asm'
include 'utils.asm'
include 'patterns.asm'
include 'aliens.asm'
include 'shield.asm'