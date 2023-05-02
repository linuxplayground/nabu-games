include 'tms_constants.asm'

tms_init_g2:
        ld      hl,.tms_init_g2_registers
        ld      b,.tms_init_g2_registers_length
        ld      c,io_tmslatch
        otir
        ret

; functions

; d = value
; b = register
tms_set_register:
        ld      a,d
        out     (io_tmslatch),a
        ld      a,b
        or      0x80
        out     (io_tmslatch),a
        ret

; color in b
tms_set_backdrop_color:
        ld      d,b
        ld      b,0x07
        call    tms_set_register
        ret

; de = write address
tms_set_write_address:
        ld      a,e
        out     (io_tmslatch),a
        ld      a,d
        or      0x40
        out     (io_tmslatch),a
        ret

; color in b
tms_set_all_colors:
        ld      de,tms_colorTable
        call    tms_set_write_address

        ld      l,b
        ld      de,tms_colorTableLen
        call    tms_set_vram_loop_start
        ret

; write a single byte to VRAM
tms_put:
        out (io_tmsdata),a
        ret

; wait for the interrupt bit to be set
; this works differently on different platforms.
; for now we just poll the tms register
tms_wait:
        in      a,(io_tmslatch)
        rlca
        jr      nc,tms_wait
        ret

; initilize all vram to 0x00
tms_clear_vram:
        ld      de,0x00
        call    tms_set_write_address

        ld      de,0x3FFF
        ld      l,0x00
        ; pass through

; write whatever is in l to vdp for de count times.
; l = value to write
; de = number of times to write it.
tms_set_vram_loop_start:
        ld      b,e
        dec     de
        inc     d
        ld      a,l
tms_set_vram_loop:
        out     (io_tmsdata),a
        push    hl
        pop     hl
        push    hl
        pop     hl
        push    hl
        pop     hl
        djnz    tms_set_vram_loop
        dec     d
        jp      nz,tms_set_vram_loop
        ret

; writes zeros to all names in name table.
tms_clear_screen:
        ld      de,tms_nameTable
        call    tms_set_write_address

        ld      de,tms_nameTableLen
        ld      l,0x00
        call    tms_set_vram_loop_start
        ret

; DE = VDP target memory address
; HL = host memory address
; BC = number of bytes to write
; Clobbers: AF, BC, DE, HL
tms_write_slow:
        call    tms_set_write_address

        ld      d,b
        ld      e,c
        ld      c,io_tmsdata
.tms_write_slow_loop:
        outi
        push    hl
        pop     hl
        push    hl
        pop     hl
        dec     de
        ld      a,d
        or      e
        jr      nz,.tms_write_slow_loop
        ret

; DE = VDP target memory address
; HL = host memory address
; BC = number of bytes to write
; Clobbers: AF, BC, DE, HL
tms_write_fast:
        call    tms_set_write_address
        ld      d,b
        ld      e,c
        ld      c,io_tmsdata
; goldilocks
        ld      b,e
        inc     e
        dec     e
        jr      z,tms_write_fast_loop
        inc     d
tms_write_fast_loop:
        outi
        jp      nz,tms_write_fast_loop
        dec     d
        jp      nz,tms_write_fast_loop
        ret

; flush the tms_buffer to the nameTable vram address space.
tms_flush_buffer:
        ld      de,tms_nameTable
        ld      hl,tms_buffer
        ld      bc,0x300
        call    tms_wait
        call    tms_write_fast
        ret

; Graphics Mode II registers
.tms_init_g2_registers:
        db      0x02, 0x80      ;Graphics mode 2, no external video
        db      0xe0, 0x81      ;16K,enable display, disable interrupts
        db      0x0e, 0x82      ;name table = 0x1800
        db      0x9f, 0x83      ;color table = 0x2000-0x2800
        db      0x00, 0x84      ;pattern table = 0x0000-0x0800
        db      0x76, 0x85      ;sprite attribute table 0x1b00
        db      0x03, 0x86      ;sprite pattern table 0x1800
        db      0x0b, 0x87      ;backdrop color = black
.tms_init_g2_registers_length: equ $-.tms_init_g2_registers

; Frame buffer for graphics mode 2
tms_buffer:     ds 0x300, 0
