addhla: macro
        add     a, l    ; A = A+L
        ld      l, a    ; L = A+L
        adc     a, h    ; A = A+L+H+carry
        sub     l       ; A = H+carry
        ld      h, a    ; H = H+carry
endm

adddea: macro
        add     a, e    ; A = A+E
        ld      e, a    ; L = A+E
        adc     a, d    ; A = A+E+D+carry
        sub     e       ; A = D+carry
        ld      d, a    ; D = D+carry
endm

mul4: macro
        add     hl,hl   ;2
        add     hl,hl   ;4
endm

mul8: macro
        add     hl,hl   ;2
        add     hl,hl   ;4
        add     hl,hl   ;8
endm

mul16: macro
        add     hl,hl   ;2
        add     hl,hl   ;4
        add     hl,hl   ;8
        add     hl,hl   ;16
endm

mul32: macro
        add     hl,hl   ;2
        add     hl,hl   ;4
        add     hl,hl   ;8
        add     hl,hl   ;16
        add     hl,hl   ;x32
endm

div2: macro
        and     0xfe       ; remove the bits that get rotated out
        rra
endm

div4: macro
        and     0xfc       ; remove the bits that get rotated out
        rra
        rra
endm

div8:   macro
        and     0xf8       ; remove the bits that get rotated out
        rra
        rra
        rra
endm

; copy data from HL to DE BC times.
memcpy: macro source destination count
        ld      hl, source
        ld      de, destination
        ld      bc, count
        ldir
endm

; fills a block of memory frpm start with value to start + count
fillmem: macro start value size
        ld      a,value
        ld      hl,start
        ld      bc,size
        call    fillmem
endm

; decrement a variable
memdec: macro variable
        ld      a,(variable)
        dec     a
        ld      (variable),a
endm

; increment a variable
meminc: macro variable
        ld      a,(variable)
        inc     a
        ld      (variable),a
endm