; Draw the invaders shield

create_shield_pattern_table:
        ld      de,shield_pattern_start
        ld      hl,SHIELD
        ld      bc,shield_pattern_len
        call    tms_write_slow
        ld      de,shield_pattern_start + 48
        ld      hl,SHIELD
        ld      bc,shield_pattern_len
        call    tms_write_slow
        ld      de,shield_pattern_start + 48 + 48
        ld      hl,SHIELD
        ld      bc,shield_pattern_len
        call    tms_write_slow
        ld      de,shield_pattern_start + 48 + 48 + 48
        ld      hl,SHIELD
        ld      bc,shield_pattern_len
        jp      tms_write_slow

draw_shield_layout:
        memcpy  shield_layout shield_buffer_start shield_layout_length
        ret


; shield layout
shield_layout:
        db      25,26,27,0,0,0,31,32,33,0,0,0,37,38,39,0,0,0,43,44,45
        db      0,0,0,0,0,0,0,0,0,0,0
        db      28,29,30,0,0,0,34,35,36,0,0,0,40,41,42,0,0,0,46,47,48
shield_layout_length:   equ $-shield_layout

shield_buffer_start:    equ tms_buffer + (19 * 32) + 6