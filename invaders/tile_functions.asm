; these functions are all to do with converting pixel co-ordinates to tiles
; and tile co-ordinates to pixel co-ordinates.
; Generally the DE register will be used to track these coordinates through
; these functions.

tile_y:         db 0    ; tile coordinate
tile_x:         db 0    ; tile coordinate
tile_px_y:      db 0    ; tile pixel offset (the pixel within a specific tile.)
tile_px_x:      db 0    ; tile pixel offset (the pixel within a specific tile.)

; convert pixel XY co-ordinates to tile_xy co-ordinates
; input: DE = D=pixel X, E=pixel Y
; output DE = D=tile X, E=tile Y (AND tile_y and tile_x are set.)
; output HL = H=tile_pix_offset X, L=tile_pix_offset Y (AND tile_px_y and tile_pix_x are set)
pixel_xy_to_tile_xy:
        ld      h,d
        ld      a,d
        div8
        ld      d,a
        ld      a,h
        and     0x07    ; mod 8
        ld      h,a

        ld      l,e
        ld      a,e
        div8
        ld      e,a
        ld      a,l
        and     0x07    ; mod 8
        ld      l,a

        ld      (tile_y),de
        ld      (tile_px_y),hl
        ret

; given the tile position x,y (stored in DE) find the value from the framebuffer.
; value is returned in A
; clobbers HL, AF
value_at_tile_xy:
        ld      l,e             ; y is now l
        ld      h,0
        mul32                   ; hl is now y * 32
        ld      a,d             ; a has value of x
        addhla                  ; add x to y * 32
        ld      de, tms_buffer  ; de is start of frame buffer in ram
        add     hl,de           ; adds hl to the start of frame buffer, saves in HL
        ld      a,(hl)          ; get value of memory at (hl)
        ret

; given a value in A find the TMS pattern from pattern generator vdp ram.
; offset by tile_px_y to fetch specific row.  Used in conjunction with pixel_xy_to_tile_xy
; returns pattern byte in A
; clobbers de
fetch_tile_pattern_row:
        ld      de,INVADER1             ; de is start of pattern table.
        adddea                          ; add the name value to the start of pattern table.
        ld      a,(tile_px_y)
        adddea                          ; de now points to the row of the pattern
        ex      de,hl
        ld      a,(hl)
        ret

; Pattern lookup table
bit0:   equ     %10000000
bit1:   equ     %01000000
bit2:   equ     %00100000
bit3:   equ     %00010000
bit4:   equ     %00001000
bit5:   equ     %00000100
bit6:   equ     %00000010
bit7:   equ     %00000001

bits:   db      bit0, bit1, bit2, bit3, bit4, bit5, bit6, bit7 ; lookup table.

; given a pattern row byte - and an x pixel offset, test if the pixel offset matches
; a set pixel in the pattern.
; input: a=8bit pattern to test
; tile_px_x is going to be a value between 0 and 7 with 0 being the MSB to test
; on the pattern.  We use the bits lookup table defined above to find the specific
; bitpattern to match.  We and the pattern row byte with the bit pattern as indexed
; by tile_px_x and if the zero flag is set if we don't have a hit
; Output is A,  Either 0 or some value. if zero no hit.
test_pattern_collide:
        ld      bc,(tile_px_x)
        ; ld      b,0             ; bc has offset into bits.
        ld      hl,bits         ; hl has start of bits
        add     hl,bc           ; hl has offset into bits 
        and     (hl)            ; and.
        ret
