invader_1_pattern_start:        equ tms_patternTable + 8
invader_2_pattern_start:        equ invader_1_pattern_start + 64
invader_3_pattern_start:        equ invader_2_pattern_start + 64

invader_1_color_start:          equ tms_colorTable + 8
invader_2_color_start:          equ invader_1_pattern_start + 64
invader_3_color_start:          equ invader_2_pattern_start + 64

shield_pattern_start:           equ invader_3_pattern_start + 64
shield_pattern_len:             equ 48

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
        jp      tms_set_vram_loop_start

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
        jp      tms_write_slow

;-------------------------------------------------------------------------------
;               D R A W    I N V A D E R S
;
;  Iterate through the array of invader structs and plot each invader with
;  type > 0 to it's X,Y coordinates.
;    get the current Y, X and T for each invaders.
;    for Aliens that are not 0:
;      X is the Pixel location.
;      Y is the tile location.
;      Convert X to X-Tile location and X-Frame Offset (/4 && %8)
;      Draw alien blank. (at original location)
;      Based on direction, either increment X-pixel or decrement X-pixel
;      Convert new X-pixel back to X-Tile location.
;      Check X-Tile for edge of screen - Change direction if so.
;      Save X Pixel back to alien struct
;      Draw alien at X-Tile location.
;    decrement logic - Loop
;-------------------------------------------------------------------------------
draw_invaders:
        ld      ix,invaders
        ld      b,55            ; there are 55 invader cells
.draw_loop:
        ; check if we need to skip
        ld      a,(ix+0)
        or      a
        jr      z,.draw_skip    ; the invader at this position is dead.  Skip it.

        ; get current x-pixel and y-tile, convert x-pixel to tile and draw alien
        ; with blank pattern
        ld      a,(ix+1)        ; x-pixel
        div4                    ; x-tile
        ld      h,a             ; save x-tile in h
        ld      a,(ix+2)        ; y-tile
        ld      l,a             ; save y-tile into h
        xor     a
        call    draw_invader    ; draw invader with blank pattern.

        ; check current direction to either increment or decrement x-pixel
        ld      a,(x_dir)
        or      a
        jp      z,.draw_scroll_left
.draw_scroll_right:
        inc     (ix+1)
        jp      .draw_test_boundary
.draw_scroll_left:
        dec     (ix+1)
.draw_test_boundary:
        ; convert (ix+1) to tile
        ld      a,(ix+1)
        div4
        cp      1
        jr      c,.draw_change_direction
        cp      30
        jr      nc,.draw_change_direction
        jp      .draw_set_new_pos
.draw_change_direction:
        ld      a,(x_dir)
        or      a
        jr      z,.draw_change_to_right
.draw_change_to_left:
        ld      a,0
        ld      (new_x_dir),a
        ld      a,1
        ld      (drop_invaders_flag),a
        jp      .draw_set_new_pos
.draw_change_to_right:
        ld      a,1
        ld      (new_x_dir),a
        ld      a,1
        ld      (drop_invaders_flag),a
        ; fall through
.draw_set_new_pos:
        ; convert (ix+1) to tile
        ld      a,(ix+1)
        div4
        ld      (ix+3),a
        ld      h,a
        ld      a,(ix+2)
        ld      l,a
.draw_set_new_pattern:
        ld      a,(ix+1)        ; use the new pixel position
        and     0x03            ; to find the correct pattern
        add     a,a             ; to apply.
        add     a,(ix+0)        ; offset the invader pattern type
        call    draw_invader

.draw_skip:
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        
        djnz    .draw_loop

        ld      a,(new_x_dir)
        ld      (x_dir),a
        ret

;-------------------------------------------------------------------------------
;               D R O P    I N V A D E R S
;-------------------------------------------------------------------------------
drop_invaders:
        ld      ix,invaders
        ld      b,55            ; there are 55 invader cells
.drop_loop:
        ; get current x-pixel and y-tile, convert x-pixel to tile and draw alien
        ; with blank pattern
        ld      a,(ix+0)
        or      a
        jp      z,.drop_loop_skip
        ld      a,(ix+3)        ; x-pixel
        ld      h,a             ; save x-tile in h
        ld      a,(ix+2)        ; y-tile
        ld      l,a             ; save y-tile into h
        xor     a
        call    draw_invader    ; draw invader with blank pattern.
        ; now shift it down ready for next draw.
        ld      a,(ix+2)
        inc     a
        ld      (ix+2),a
        cp      22
        jp      nc,cpm_terminate

        ; convert (ix+1) to tile
        ld      a,(ix+3)
        ld      h,a
        ld      a,(ix+2)
        ld      l,a

        ld      a,(ix+1)        ; use the new pixel position
        and     0x03            ; to find the correct pattern
        add     a,a             ; to apply.
        add     a,(ix+0)        ; offset the invader pattern type
        call    draw_invader
.drop_loop_skip:
        inc     ix
        inc     ix
        inc     ix
        inc     ix


        djnz    .drop_loop
        xor     a
        ld      (drop_invaders_flag),a
        ret



; A = tile pattern, hl = x,y
draw_invader:
        ld      c,a
        ld      d,h
        ld      h,0
        mul32
        ld      a,d
        addhla
        ld      de,tms_buffer
        add     hl,de
        ld      a,c
        ld      (hl),a
        inc     hl
        or      a
        jr      z,.draw_invader_skip_blank
        inc     a
.draw_invader_skip_blank:
        ld      (hl),a
        ret

;-------------------------------------------------------------------------------
;               C H A N G E    D I R E C T I O N
;
;  Changes the direction of X_DIR
;  Also drops each alien down by 1 row (inrement Y for each alien)
;-------------------------------------------------------------------------------
change_direction_to_right:
        ld      a,(x_dir)
        cp      -1
        jp      nz,.no_change
        ld      a,1
        jp      .change_direction_drop_row
change_direction_to_left:
        ld      a,(x_dir)
        cp      1
        jp      nz,.no_change
        ld      a,-1
.change_direction_drop_row:
        ld      (x_dir),a
        ; TODO
.no_change:
        ret

;-------------------------------------------------------------------------------
;               I N V A D E R   S T R U C T
;
;  The invaders array is formed of 55 structs defined as follows:
;
;  T = enum[0x00|0x01|0x09|0x17] Type of Invader.  0x00 means invisible
;  PX = int(8) X position (PIXEL)
;  TY = int(8) Y position (TILE)
;  TX = int(8) X position (PIXEL)
;  This leaves 55*3 memory locations.
;-------------------------------------------------------------------------------
;                        T     PX    TY    TX
invaders:       db      0x11, 0x09, 0x02, 0x00
                db      0x11, 0x11, 0x02, 0x00
                db      0x11, 0x19, 0x02, 0x00
                db      0x11, 0x21, 0x02, 0x00
                db      0x11, 0x29, 0x02, 0x00
                db      0x11, 0x31, 0x02, 0x00
                db      0x11, 0x39, 0x02, 0x00
                db      0x11, 0x41, 0x02, 0x00
                db      0x11, 0x49, 0x02, 0x00
                db      0x11, 0x51, 0x02, 0x00
                db      0x11, 0x59, 0x02, 0x00

                db      0x09, 0x09, 0x04, 0x00
                db      0x09, 0x11, 0x04, 0x00
                db      0x09, 0x19, 0x04, 0x00
                db      0x09, 0x21, 0x04, 0x00
                db      0x09, 0x29, 0x04, 0x00
                db      0x09, 0x31, 0x04, 0x00
                db      0x09, 0x39, 0x04, 0x00
                db      0x09, 0x41, 0x04, 0x00
                db      0x09, 0x49, 0x04, 0x00
                db      0x09, 0x51, 0x04, 0x00
                db      0x09, 0x59, 0x04, 0x00

                db      0x09, 0x09, 0x06, 0x00
                db      0x09, 0x11, 0x06, 0x00
                db      0x09, 0x19, 0x06, 0x00
                db      0x09, 0x21, 0x06, 0x00
                db      0x09, 0x29, 0x06, 0x00
                db      0x09, 0x31, 0x06, 0x00
                db      0x09, 0x39, 0x06, 0x00
                db      0x09, 0x41, 0x06, 0x00
                db      0x09, 0x49, 0x06, 0x00
                db      0x09, 0x51, 0x06, 0x00
                db      0x09, 0x59, 0x06, 0x00

                db      0x01, 0x09, 0x08, 0x00
                db      0x01, 0x11, 0x08, 0x00
                db      0x01, 0x19, 0x08, 0x00
                db      0x01, 0x21, 0x08, 0x00
                db      0x01, 0x29, 0x08, 0x00
                db      0x01, 0x31, 0x08, 0x00
                db      0x01, 0x39, 0x08, 0x00
                db      0x01, 0x41, 0x08, 0x00
                db      0x01, 0x49, 0x08, 0x00
                db      0x01, 0x51, 0x08, 0x00
                db      0x01, 0x59, 0x08, 0x00

                db      0x01, 0x09, 0x0A, 0x00
                db      0x01, 0x11, 0x0A, 0x00
                db      0x01, 0x19, 0x0A, 0x00
                db      0x01, 0x21, 0x0A, 0x00
                db      0x01, 0x29, 0x0A, 0x00
                db      0x01, 0x31, 0x0A, 0x00
                db      0x01, 0x39, 0x0A, 0x00
                db      0x01, 0x41, 0x0A, 0x00
                db      0x01, 0x49, 0x0A, 0x00
                db      0x01, 0x51, 0x0A, 0x00
                db      0x01, 0x59, 0x0A, 0x00
