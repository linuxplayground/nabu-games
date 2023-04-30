; everything to do with plotting aliens into the frame buffer
;

game_field_buffer_start:        equ tms_buffer + 0x40
game_field_buffer_size:         equ 0x260

; set up invader colors
create_invader_color_table:
        ld      de,tms_colorTable + 8
        call    tms_set_write_address
        ld      de, 64
        ld      hl,(INVADER1_COLOR)
        call    tms_set_vram_loop_start
        ld      de, 64
        ld      hl,(INVADER2_COLOR)
        call    tms_set_vram_loop_start
        ld      de, 64
        ld      hl,(INVADER3_COLOR)
        call    tms_set_vram_loop_start
        ret

; create invader pattern table
create_invader_pattern_table:
        ld      de,8
        ld      hl,INVADER1
        ld      bc,64
        call    tms_write_fast
        ld      de,8+64
        ld      hl,INVADER2
        ld      bc,64
        call    tms_write_fast
        ld      de,8+64+64
        ld      hl,INVADER3
        ld      bc,64
        call    tms_write_fast

; use the gamefield data to position the aliens on the frame buffer.
; game_field_offset % 4 + value of gamefield.
update_game_field:
        ld      de,game_field_buffer_start
        ld      a,(game_y_offset)
        ld      l,a
        ld      h,0
        mul32
        add     hl,de
        ex      de,hl
        ld      a,(game_x_offset)
        sra     a
        sra     a       ; div 4
        ld      (game_field_offset),a
        adddea

        ld      hl,game_field
        ld      bc,game_field_size
update_game_field_1:
        ; we need to check if gamefield at (hl) is a 0
        ld      a,(hl)
        or      a
        jp      z,.draw_blank

        ; get the value at (hl)
        ld      a,(game_x_offset)
        and     0x03            ; mod 4
        sla     a               ; x 2

.continue:
        add     a,(hl)
        ; draw the tile
        ld      (de),a          ;left side of alien
        inc     de              ;increment buffer pointer
        inc     a               ; on both sides of the alien.
        ld      (de),a          ;right side of alien
        inc     de              ; bring de to beginning of next row
        jp      .decrement_logic
.draw_blank:
        xor     a
        ld      (de),a
        inc     de
        ld      (de),a
        inc     de
        ;fall through
.decrement_logic:
        inc     hl
        dec     bc
        ld      a,b
        or      c
        jp      nz,update_game_field_1
        ret

; initial game field - lays out the initial alien pattern on the left of the
; screen.
game_field:    ;0    2   4   6   8  10  12  14  16  18  20  22 24 28 30 32
        db      17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  0, 0, 0, 0, 0  ; 0
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 1
        db       9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0, 0, 0, 0, 0  ; 2
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 3
        db       9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0, 0, 0, 0, 0  ; 4
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 5
        db       1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0, 0, 0, 0, 0  ; 6
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 7
        db       1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0, 0, 0, 0, 0  ; 8
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 9
game_field_size: equ $-game_field
