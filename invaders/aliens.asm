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


; initial game field - lays out the initial alien pattern on the left of the
; screen.
init_game_field:    ;0    2   4   6   8  10  12  14  16  18  20  22 24 28 30 32
        db           17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  0, 0, 0, 0, 0  ; 0
        db            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 1
        db            9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0, 0, 0, 0, 0  ; 2
        db            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 3
        db            9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0, 0, 0, 0, 0  ; 4
        db            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 5
        db            1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0, 0, 0, 0, 0  ; 6
        db            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0  ; 7
        db            1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1                  ; 8
init_game_field_size:   equ $-init_game_field

game_field:             ds init_game_field_size, 0
game_field_size:        equ init_game_field_size
