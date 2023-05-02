; everything to do with plotting aliens into the frame buffer
;

game_field_buffer_start:        equ tms_buffer + 0x40
game_field_buffer_size:         equ game_field_size * 2 + 2

invader_1_pattern_start:        equ tms_patternTable + 8
invader_2_pattern_start:        equ invader_1_pattern_start + 64
invader_3_pattern_start:        equ invader_2_pattern_start + 64

invader_1_color_start:          equ tms_colorTable + 8
invader_2_color_start:          equ invader_1_pattern_start + 64
invader_3_color_start:          equ invader_2_pattern_start + 64

shield_pattern_start:           equ invader_3_pattern_start + 64
shield_pattern_len:             equ 48

; set up gamefield from defaults
create_game_field:
        memcpy init_game_field game_field init_game_field_size
        ret

; set up invader colors
create_invader_color_table:
        ld      de,invader_1_color_start
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
        ld      de,invader_1_pattern_start
        ld      hl,INVADER1
        ld      bc,64
        call    tms_write_slow
        ld      de,invader_2_pattern_start
        ld      hl,INVADER2
        ld      bc,64
        call    tms_write_slow
        ld      de,invader_3_pattern_start
        ld      hl,INVADER3
        ld      bc,64
        call    tms_write_slow

; clear the row directly above the top of the set of aliens
; all the way through to the row immediately below the set of aliens.
.clear_game_field:
        ; find start of buffer based on y offset.
        ld      de,game_field_buffer_start
        ld      a,(game_y_offset)
        dec     a
        ld      l,a
        ld      h,0
        mul32
        add     hl,de
        push    hl                              ; save hl

        ; find number of rows to clear based on game_rows
        ld      a,(game_rows)
        inc     a                               ; make sure we go for one row beyond the bottom of aliens.
        ld      l,a
        ld      h,0
        mul32
        ld      c,l
        ld      b,h                             ; set bc = hl
        pop     hl                              ; restore hl
        ld      a,0x00
        call    fillmem
        ret

; use the gamefield data to position the aliens on the frame buffer.
; game_field_offset % 4 + value of gamefield.
update_game_field:
        call    .clear_game_field
        ld      de,game_field_buffer_start
        ld      a,(game_y_offset)
        ld      l,a
        ld      h,0
        mul32
        add     hl,de
        ex      de,hl
        ld      a,(game_x_offset)
        sra     a
        sra     a       ; div 4 because aliens are two tiles each.  otherwise it would have been div 8
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
        ; xor     a
        ; ld      (de),a
        inc     de
        ; ld      (de),a
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
init_game_field:    ;0    2   4   6   8  10  12  14  16  18  20  22 24 28 30 32
        db      17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  0, 0, 0, 0, 0  ; 0
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 1
        db       9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0, 0, 0, 0, 0  ; 2
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 3
        db       9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0, 0, 0, 0, 0  ; 4
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 5
        db       1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0, 0, 0, 0, 0  ; 6
        db       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 7
        db       1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1                  ; 8
init_game_field_size:   equ $-init_game_field

game_field:             ds init_game_field_size, 0
game_field_size:        equ init_game_field_size
