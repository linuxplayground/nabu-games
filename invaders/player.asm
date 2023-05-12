; player sprite setup
; player movement
; shooting

SPRITE_PLAYER:          equ     0
SRPITE_BULLET:          equ     1

PLAYER_ATTRIBUTES:      db      21*8            ; ypos
                        db      256/2-(16/2)    ; xpos
                        db      0               ; pattern
                        db      tms_white       ; color
PLAYER_ATTRIBUTE_LEN:   equ $-PLAYER_ATTRIBUTES

BULLET_ATTRIBUTES:      db      0xd0            ; ypos - D0 means do not display
                        db      256/2-(16/2)    ; xpos
                        db      4               ; pattern
                        db      tms_cyan  ; color
BULLET_ATTRIBUTE_LEN:   equ $-BULLET_ATTRIBUTES

; set up the player sprite sp-1
create_player_sprite:
        ld      de,tms_spritePatternTable
        ld      bc,32
        ld      hl,playerSprite
        call    tms_write_slow        
        ret


set_player_attributes:
        ld      de, tms_spriteAttributeTable + (SPRITE_PLAYER * 4)
        ld      hl, PLAYER_ATTRIBUTES
        ld      bc, PLAYER_ATTRIBUTE_LEN
        call    tms_write_slow
        ret
; set up initial player bullet sprite
create_bullet_sprite:
        ld      de,tms_spritePatternTable + (SRPITE_BULLET*4*8)
        ld      bc,32
        ld      hl,bulletSprite
        call    tms_write_slow
        ret

set_bullet_attributes:
        ld      de, tms_spriteAttributeTable + (SRPITE_BULLET * 4)
        ld      hl, BULLET_ATTRIBUTES
        ld      bc, BULLET_ATTRIBUTE_LEN
        call    tms_write_slow
        ret

move_player_left:
        ld      a,(PLAYER_ATTRIBUTES + 1)
        sub     player_speed
        ld      (PLAYER_ATTRIBUTES + 1),a
.move_player_left_exit:
        ret

move_player_right:
        ld      a,(PLAYER_ATTRIBUTES + 1)
        add     player_speed
        ld      (PLAYER_ATTRIBUTES + 1),a
.move_player_right_exit:
        ret

animate_bullet:
        ld      a,(bullet_active)
        or      a
        jr      z,.animate_bullet_nop
        ld      a,(BULLET_ATTRIBUTES)
        sub     bullet_speed
        jp      c,.disable_bullet
        ld      (BULLET_ATTRIBUTES),a
        jr      .animate_bullet_exit
.disable_bullet:
        ld      a,0xd0
        ld      (BULLET_ATTRIBUTES),a
        xor     a
        ld      (bullet_active),a
.animate_bullet_exit:
        call    test_bullet_hit
        call    set_bullet_attributes    
.animate_bullet_nop:  
        ret

; check to see if the bullet is overlapping a tile we care about
test_bullet_hit:
        ; push    hl
        ; push    de
        ; push    bc

        ; find the tile and pixel offset for the bullet.
        ; find the pixel_y_offset and the pixel_x_offset for the bullet pixel location
        
        ; y_tile
        ld      c,a
        div8
        ld      (bullet_t_y),a
        ; y_pixel offset
        ld      a,c
        and     0x07

        ld      (bullet_px_y),a
        
        ; x_tile
        ld      a,(BULLET_ATTRIBUTES + 1)
        ld      c,a
        div8
        inc     a
        dec     a
        ld      (bullet_t_x),a
        ; x_pixel offset
        ld      a,c
        and     0x07
        ld      (bullet_px_x),a

        ; convert the tile_x, tile_y to a tms_buffer offset.
        ld      hl,(bullet_t_y)
        ld      h,0
        mul32
        ld      a,(bullet_t_x)
        addhla
        ld      de,tms_buffer
        add     hl,de

        ; read the data from the buffer at the buffer offset into VAL
        ld      a,(hl)
        ; if in range of 1 to 1f then an invader could have been hit.
        or      a
        jr      z,.test_tile_hit_exit           ; did not hit any tile.
        cp      25
        jr      nc,.not_alien_pattern
        ; read the pattern data for the row identified by VAL + bullet_px_y into PAT
        ; a = pattern name
        dec     a       ;reduce by 1 because maths.  (If pattern name is a 2 we want to offset from invaders1 by 1x8 not 2x8 to find start of pattern)
        add     a
        add     a
        add     a            ;x8 get the offset into the pattern table starting at invader1
        ld      hl,INVADER1
        addhla
        ; get the pattern row offset
        ld      a,(bullet_px_y)
        addhla
        ld      a,(hl)
        ld      c,a     ; c now has the pattern row bits
        ; get the mask bit offset
        ld      a,(bullet_px_x)
        ld      hl,bullet_hit_bits
        addhla          ; hl points to the bitmask we want based on x position of the bullet
        ld      a,(hl)  ; a is the bitmask
        and     c       ; a AND c
        jr      z,.not_pixel_hit
        ; INVADER HIT CONFIRMED

        ; subtract x_t_offset from tile_x
        ld      a,(x_t_offset)
        inc     a
        ld      c,a
        ld      a,(bullet_t_x)
        sub     c
        ; divide by 2 result is gamefield_x_offset
        div2
        ld      (game_field_x),a

        ; subtract y_t_offset from tile_y
        ld      a,(y_t_offset)
        ld      c,a
        ld      a,(bullet_t_y)
        sub     c
        ; divide by 2 result is gamefield_y_offset
        div2
        ld      (game_field_y),a                ; use this later to check if row was cleared.
        ; use gamefield_(xy)_offsets to calculate gamefield_ram_offset
        ld      l,a
        ld      h,0
        mul16
        ld      a,(game_field_x)
        addhla
        ld      de,game_field
        add     hl,de
        ; set data at gamefield + gamefield_ram_offset  = 0x00
        xor     a
        ld      (hl),a
        call    update_game_field
        ld      a,0xd0
        ld      (BULLET_ATTRIBUTES),a
        xor     a
        ld      (bullet_active),a
        ; XXX THERE IS AN EDGE CASE BUG HERE.   If one is able to clear a row that's not the bottom row. then game rows will be decremented.
        ; we only want to decrement game rows when the bottom most row of aliens is cleared.
        call    check_row_clear
        jp      set_bullet_attributes
.not_pixel_hit:
.not_alien_pattern:
.test_tile_hit_exit:
        ret

; Variables
bullet_active:  db 0

bullet_t_x:     db 0    ;bullet tile location x
bullet_t_y:     db 0    ;bullet tile location y
bullet_px_x:    db 0    ;bullet pixel offset x
bullet_px_y:    db 0    ;bullet pixel offset y

bullet_hit_bits:        
        db %10000000
        db %01000000
        db %00100000
        db %00010000
        db %00001000
        db %00000100
        db %00000010
        db %00000001