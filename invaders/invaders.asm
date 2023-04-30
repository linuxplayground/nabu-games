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
        
; init gamefield (aliens)
        ld      a,8
        ld      (game_x_offset),a
        ld      a,2
        ld      (game_y_offset),a
        ld      a,1
        ld      (x_dir),a
; copy initial gamefield data over to variable gamefield data for game play.
; this will allow us to later on alter the values in the gamefield array.
        ; memcpy  initial_game_field game_field initial_game_field_size

        fillmem tms_buffer 0x00 0x300           ; init framebuffer
        call    update_game_field               ; draw aliens in initial positions.
        call    tms_wait                        ; wait for vsync status register on vdp
        call    tms_flush_buffer                ; write framebuffer to the vdp.

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
        cp      11                              ; check for bottom boundary
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
        cp      11                              ; if bottom boundary hit, reset game.
        jp      p,.reset_game
        ; fall through
.flush:
        xor     a
        ld      (ticks),a                       ; reset tick count to 0.
        ; call    hexdump_a
        fillmem game_field_buffer_start 0x00 game_field_buffer_size ; clear game field in the buffer.
        call    update_game_field               ; draw the current game field on to the buffer.
.end_loop:
        call    tms_wait                        ; wait for vsync
        call    tms_flush_buffer                ; fast flush the buffer to to the vdp.
        jp      .gameloop

; Move all the aliens back to their starting posistions.
.reset_game:
        call    create_game_field
        ld      a,2                             ; set first row to 2 down from top.
        ld      (game_y_offset),a
        ld      a,8
        ld      (game_x_offset),a
        ld      a,1
        ld      (x_dir),a
        jp      .gameloop                       ; enter game.

; initialize variables
ticks:                  db 0
x_dir:                  db 0
game_field_offset:      db 0
game_x_offset:          db 0
game_y_offset:          db 0

include 'tms.asm'
include 'utils.asm'
include 'patterns.asm'
include 'aliens.asm'
