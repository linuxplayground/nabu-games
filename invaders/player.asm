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

; set up the player sprite sp-1
create_player_sprite:
        ld      de,tms_spritePatternTable
        ld      bc,32
        ld      hl,playerSprite
        call    tms_write_slow        
        ret

; set initial position and color of sprite
set_player_attributes:
        ld      de, tms_spriteAttributeTable + (SPRITE_PLAYER * 4)
        ld      hl, PLAYER_ATTRIBUTES
        ld      bc, PLAYER_ATTRIBUTE_LEN
        call    tms_wait
        call    tms_write_fast
        ret

move_player_left:
        ld      a,(PLAYER_ATTRIBUTES+1)
        dec     a
        dec     a
        ld      (PLAYER_ATTRIBUTES+1),a
        call    set_player_attributes
        ret

move_player_right:
        ld      a,(PLAYER_ATTRIBUTES+1)
        inc     a
        inc     a
        ld      (PLAYER_ATTRIBUTES+1),a
        call    set_player_attributes
        ret