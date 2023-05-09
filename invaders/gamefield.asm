; everything to do with plotting aliens into the frame buffer
;

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

; update the gamefield based on x and y offsets
; loads the alien patterns into the tms buffer
update_game_field:
        ; get the start location in the tms buffer for the first alien.
        ; y offset * 32 + x offset (in tiles)
        ld      hl,(y_t_offset)
        ld      h,0
        ld      (.tmp_y),hl              ; keep track of row as we work our way through the gamefield rows.
        mul32
        ld      a,(x_t_offset)
        addhla
        ld      de,tms_buffer
        add     hl,de
        ex      de,hl                   ; de now has start of game field in tms buffer

        ld      hl,game_field           ; hl now has start of gamefield array

        ld      a,(game_rows)
        ld      b,a                     ; use b to count game rows
.update_game_field_row_loop:
        ld      a,(game_cols)
        ld      c,a                     ; use c to count game cols
        ; first col is blank
        xor     a
        ld      (de),a
        inc     de
.update_game_field_col_loop:
        ld      a,(hl)
        or      a
        jr      z,.update_game_field_write
        ld      a,(x_px_offset)
        and     0x03
        add     a,a
        add     (hl)
.update_game_field_write:
        ld      (de),a
        or      a
        jr      z,.update_game_field_char_right
        inc     a
.update_game_field_char_right:
        inc     de
        ld      (de),a
        ; advance to next column
        inc     hl
        inc     de
        ; check if we are done with row
        dec     c
        jr      nz,.update_game_field_col_loop
        ; done with row - 
        ; draw one last blank column
        xor     a
        ld      (de),a
        ; move .tmp_y to two rows down and then use it to calculate the new de
        dec     b
        jp      z,.update_game_field_exit       ; if we have done all rows we can leave this routine.
        ld      a,(.tmp_y)
        inc     a
        inc     a
        ld      (.tmp_y),a
        push    hl              ; we need to save HL while we use it to do some maths
        ld      l,a
        ld      h,0             ; hl = .tmp_y
        mul32                   ; hl = .tmp_y * 32
        ld      a,(x_t_offset)
        addhla
        ld      de,tms_buffer
        add     hl,de
        ex      de,hl           ; de = .tmp_y * 32
        pop     hl              ; restore hl pointer into game field
        ld      a,5
        addhla
        jp      .update_game_field_row_loop
.update_game_field_exit:
        ret

; clear the whole game field from y_t_offset - 1 to y_t_offset + game_rows
clear_game_field:
        push    de
        push    hl
        ; figure out the number of bytes to fill.  (all rows)
        ld      hl,(game_rows)
        ld      h,0
        add     hl,hl
        mul32
        push    hl
        pop     bc

        ; find the start address inside the buffer for the first gamefield location.
        ld      de,tms_buffer
        ld      hl,(y_t_offset)
        ld      h,0
        dec     l
        mul32
        add     hl,de

        ld      a,0x00
        call    fillmem

        pop     hl
        pop     de
        ret

; initial game field - lays out the initial alien pattern on the left of the
; screen.
init_game_field:    ;0    2   4   6   8  10  12  14  16  18  20
        db           17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,0,0,0,0,0
        db            9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,0,0,0,0,0
        db            9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,0,0,0,0,0
        db            1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,0,0,0,0,0
        db            1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1
init_game_field_size:   equ $-init_game_field

game_field:             ds init_game_field_size, 0
game_field_size:        equ init_game_field_size

x_px_offset:    db 0            ; x offset in pixels
x_t_offset:     db 0            ; x offset in tiles
y_t_offset:     db 2            ; y offset in tiles
x_dir:          db 1            ; direction of alien movement - 0=left, 1=right
game_cols:      db 11           ; number of columns in the gamefield
game_rows:      db 5            ; number of rows in the gamefield
.tmp_y:         db 0            ; used in update_gamefield
game_field_x:   db 0            ; x offset into gamefield
game_field_y:   db 0            ; y offset into gamefield
