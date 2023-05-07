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
        ; call    tms_wait
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
        call    tms_write_fast
        ret

move_player_left:
        ld      a,(PLAYER_ATTRIBUTES+1)
        dec     a
        dec     a
        ld      c,a
        sub     a,4
        jr      z,.move_player_left_exit
        ld      a,c
        ld      (PLAYER_ATTRIBUTES+1),a
        jp      set_player_attributes
.move_player_left_exit:
        inc     a
        inc     a
        ret

move_player_right:
        ld      a,(PLAYER_ATTRIBUTES+1)
        inc     a
        inc     a
        ld      c,a
        cp      255-20
        jr      nc,.move_player_right_exit
        ld      a,c
        ld      (PLAYER_ATTRIBUTES+1),a
        jp      set_player_attributes
.move_player_right_exit:
        dec     a
        dec     a
        ret

; move the bullet sprite into position based on player location.
player_shoot:
        ld      de,(BULLET_ATTRIBUTES)
        ld      a,(bullet_in_flight)
        or      a
        jp      nz,.player_shoot_exit
        ld      a,(PLAYER_ATTRIBUTES+1)
        add     8                       ; find the midpoint of the player
        ld      (BULLET_ATTRIBUTES+1),a ; set x
        ld      a,21*8
        ld      (BULLET_ATTRIBUTES+0),a ; set y
        ld      a,1
        ld      (bullet_in_flight),a    ; flag the bullet as needing to be animated.
        jp      set_bullet_attributes
.player_shoot_exit:
        ret

kill_bullet:
        xor     a
        ld      (bullet_in_flight),a
        ld      a,0xd0
        ld      (BULLET_ATTRIBUTES+0),a ; set to hidden.
        jp      set_bullet_attributes

animate_bullet:
        ; do we even have a bullet?
        ld      a,(bullet_in_flight)
        or      a
        jr      z,.animate_bullet_exit
        ; yes we do.  Change it's y pos
        ld      a,(BULLET_ATTRIBUTES+0)
        sub     4
        ld      (BULLET_ATTRIBUTES+0),a
        ; test if bullet is off the top of the screen.
        ld      a,(BULLET_ATTRIBUTES+0)
        or      a
        jp      nz,set_bullet_attributes
        ; otherwise set bullet in flight flag to 0
        jp      kill_bullet
.animate_bullet_exit:
        ret

; check if the current bullet position matches a pixel of a tile.
test_bullet_hit:
        di
        ld      de,(BULLET_ATTRIBUTES)
        call    pixel_xy_to_tile_xy
        call    value_at_tile_xy
        or      a
        jr      z,.exit_test_bullet_hit
        ; a non 0 tile has been found
        call    fetch_tile_pattern_row
        ; if pattern is a 0x00, then skip next bit.
        or      a
        jp      z,.exit_test_bullet_hit
        call    test_pattern_collide
        jr      z,.exit_test_bullet_hit
        call    kill_bullet
        ld      de,(tile_y)
        call    tile_xy_to_gamefield_xy
        call    kill_object_at
.exit_test_bullet_hit:
        ei
        ret