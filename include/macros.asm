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

mul16: macro
        add     a,a     ; x2
        add     a,a     ; x4
        add     a,a     ; x8
        add     a,a     ; x16
endm

mul32: macro
        add     hl,hl
        add     hl,hl
        add     hl,hl
        add     hl,hl
        add     hl,hl   ; x32
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
