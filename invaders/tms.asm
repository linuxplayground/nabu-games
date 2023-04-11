; TMS9918A graphics subroutines
; Copyright 2018-2020 J.B. Langston
;
; Permission is hereby granted, free of charge, to any person obtaining a 
; copy of this software and associated documentation files (the "Software"), 
; to deal in the Software without restriction, including without limitation 
; the rights to use, copy, modify, merge, publish, distribute, sublicense, 
; and/or sell copies of the Software, and to permit persons to whom the 
; Software is furnished to do so, subject to the following conditions:
; 
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
; 
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
; FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
; DEALINGS IN THE SOFTWARE.

; VDP Programmer's Guide: http://map.grauw.nl/resources/video/ti-vdp-programmers-guide.pdf

; ---------------------------------------------------------------------------
; configuration parameters; can be changed at runtime
TmsPort:
        defb    0xa0            ; port for TMS vram (reg is 1 higher)
TmsWait:
        defb    31              ; iterations to wait after ram access
TmsMode:
        defw    0               ; mode registers
TmsNameAddr:
        defw    3800h           ; name table address (multiples of 400H)
TmsColorAddr:
        defw    2000h           ; color table address (multiples of 40H)
TmsPatternAddr:
        defw    0               ; pattern table (multiples of 800H)
TmsSpritePatternAddr:
        defw    1800h           ; sprite attribute table (multiples of 80H)
TmsSpriteAttrAddr:
        defw    3bc0h           ; sprite pattern table (multiples of 800H)
TmsScreenColors:
        defb    0               ; background (low nybble), text color (high nybble)
; ---------------------------------------------------------------------------
; register constants

TmsWriteBit:            equ 40h                 ; bit to indicate memory write

; Registers
TmsCtrl0Reg:            equ 80h
TmsCtrl1Reg:            equ 81h
TmsNameReg:             equ 82h
TmsColorTableReg:       equ 83h
TmsPatternReg:          equ 84h
TmsSpriteAttrReg:       equ 85h
TmsSpritePatternReg:    equ 86h
TmsColorReg:            equ 87h

; Control Register Bits
TmsM3:                  equ 200h
TmsExtVideo:            equ 100h
Tms16k:                 equ 80h
TmsDisplayEnable:       equ 40h
TmsIntEnableBit:        equ 20h
TmsM1:                  equ 10h
TmsM2:                  equ 8
TmsSprite32:            equ 2
TmsSpriteMag:           equ 1

; ---------------------------------------------------------------------------
; table lengths

TmsTileNameLen:         equ 300h
TmsTextNameLen:         equ 3c0h

TmsTileColorLen:        equ 20h
TmsBitmapColorLen:      equ 800h

TmsTilePatternLen:      equ 800h
TmsTextPatternLen:      equ 800h
TmsMulticolorPatternLen: equ 600h
TmsBitmapPatternLen:    equ 1800h

; ---------------------------------------------------------------------------
; color constants

TmsTransparent:         equ 0
TmsBlack:               equ 1
TmsMediumGreen:         equ 2
TmsLightGreen:          equ 3
TmsDarkBlue:            equ 4
TmsLightBlue:           equ 5
TmsDarkRed:             equ 6
TmsCyan:                equ 7
TmsMediumRed:           equ 8
TmsLightRed:            equ 9
TmsDarkYellow:          equ 0ah
TmsLightYellow:         equ 0bh
TmsDarkGreen:           equ 0ch
TmsMagenta:             equ 0dh
TmsGray:                equ 0eh
TmsWhite:               equ 0fh

; ---------------------------------------------------------------------------
; port I/O routines

; These routines access the ports configured in TmsPort.

; These memory locations can be set at runtime to support different hardware
; configurations from the same binary. TmsProbe automatically detects the
; TMS9918A on common ports.

; The TMS9918A RAM must not be accessed more than once every 8 us or display 
; corruption may occur.  During vblank and with the display disabled, 
; accesses can be 2 us apart, but we will always use 8 us minimum delay.

; TmsRamIn/TmsRamOut include a configurable delay loop, which waits for the
; configured iterations between VRAM writes to work properly with faster CPUs

; Minimum time to execute each procedure call:
; Z80: 88 cycles, 8.8 us @ 10 MHz
; Z180: 80 cycles, 8.64 us @ 9.216 MHz, 4.32 us @ 18.432, 2.16 us @ 36.864
;
; Additional delay per djnz iteration:
; Z80: 8 cycles * (iterations - 1)
;       0.8 us @ 10 MHz
; Z180: 7 cycles * (iterations - 1)
;       0.756 us @ 9.216 MHz, 0.378 us @ 18.432, 0.189 us @ 36.864

; Delay loop iterations required for different CPU speeds:
; Z80 @ 10 MHz or less: 1
; Z180 @ 9.216 MHz or less: 1
; Z180 @ 18.432 MHz: 10
; Z180 @ 36.864 MHz: 31
TmsWaits:       defb 1, 10, 31          ; wait iterations to add for different CPU speeds

; set up wait time based on clock multiplier in E
TmsSetWait:
        ld      hl, TmsWaits
        ld      d, 0
        add     hl, de
        ld      a, (hl)
        ld      (TmsWait), a
        ret

; try to find TMS9918A on common ports
TmsProbe:
        ld      hl, TmsPorts
        ld      b, TmsNumPorts
TmsProbeNext:       
        ld      a, (hl)
        ld      (TmsPort), a
        call    TmsRegIn                ; clear vsync bit
        call    TmsRegIn                ; check it again
        jp      m, TmsProbeFailed       ; if still set, not a TMS9918A
        ld      de, 0ffffh              ; long enough for another vsync
TmsProbeWait:
        call    TmsRegIn                ; check vsync bit again
        ret     m                       ; if set, it's a TMS9918A (and Z is clear)
        dec     de                      ; otherwise, keep waiting
        ld      a, e
        or      d
        jp      nz, TmsProbeWait
TmsProbeFailed:
        inc     hl                      ; if still clear after this long, try next port
        djnz    TmsProbeNext
        xor     a                       ; set Z if we ran out of ports to check
        ret

TmsPorts:                               ; List of ports to probe:
        defb 0xa0                       ; Nabu
        defb 0x80                       ; Z80-Retro
        ; add additional ports to check here
TmsNumPorts:    equ $ - TmsPorts

; set a single register value
;       A = register value
;       E = register to set
TmsSetReg:
        call    TmsRegOut
        ld      a, e
        ; fallthrough to TmsRegOut

; write to configured register port
; parameters:
;       A = value to write
TmsRegOut:
        push    bc
        ld      bc, (TmsPort)
        inc     c
        out     (c), a
        pop     bc
        ret

; read from configured register port
; returns:
;       A = value read
TmsRegIn:
        push    bc
        ld      bc, (TmsPort)
        inc     c
        in      a, (c)
        pop     bc
        ret

; write to configured VRAM port
; parameters:
;       A = value to write
                                        ; Z80 | Z180 cycles...
TmsRamOut:                              ; 17  | 16 (call)
        push    bc                      ; 11  | 11
        ld      bc, (TmsPort)           ; 20  | 18
        out     (c), a                  ; 12  | 10
TmsRamOutDelay:
        djnz    TmsRamOutDelay          ; 8   | 7  plus (13 | 9) * (iterations-1)
        pop     bc                      ; 10  | 9
        ret                             ; 10  | 9

; read from configured VRAM port
; returns:
;       A = value read
TmsRamIn:
        push    bc
        ld      bc, (TmsPort)
TmsRamInDelay:
        djnz    TmsRamInDelay
        in      a, (c)
        ld      bc, (TmsPort)
TmsRamInDelay2:
        djnz    TmsRamInDelay2
        pop     bc
        ret

; ---------------------------------------------------------------------------
; register configuration routines

; set the background color
;       A = requested color
TmsBackground:
        and     0fh
        ld      b, a
        ld      a, (TmsScreenColors)
        and     0f0h
        or      b
        ld      (TmsScreenColors), a
        ld      e, TmsColorReg
        jp      TmsSetReg

; set the sprite configuration
;       A = sprite options
TmsSpriteConfig:
        and     TmsSprite32|TmsSpriteMag
        ld      b, a
        ld      a, (TmsMode)
        and     ~(TmsSprite32|TmsSpriteMag)
        or      b
        ld      (TmsMode), a
        ld      e, TmsCtrl1Reg
        jp      TmsSetReg


; enable vblank interrupts
TmsIntEnable:
        ld      a, (TmsMode)
        or      TmsIntEnableBit
        ld      (TmsMode), a
        ld      e, TmsCtrl1Reg
        jp      TmsSetReg

; disable vblank interrupts
TmsIntDisable:
        ld      a, (TmsMode)
        and     ~TmsIntEnableBit
        ld      (TmsMode), a
        ld      e, TmsCtrl1Reg
        jp      TmsSetReg

; ---------------------------------------------------------------------------
; initialization routines

TmsBlankFlags:          equ     Tms16k
TmsTileFlags:           equ     Tms16k | TmsDisplayEnable
TmsTextFlags:           equ     Tms16k | TmsDisplayEnable | TmsM1
TmsMulticolorFlags:     equ     Tms16k | TmsDisplayEnable | TmsM2
TmsBitmapFlags:         equ     Tms16k | TmsDisplayEnable | TmsM3

; reset registers and clear all 16KB of video memory
TmsReset:
        ld      hl, TmsBlankFlags       ; blank the screen with 16KB enabled
        ld      (TmsMode), hl
        ld      a, l
        ld      e, TmsCtrl1Reg
        call    TmsSetReg
        ld      a, h
        ld      e, TmsCtrl0Reg
        call    TmsSetReg
        ld      a, TmsTransparent
        call    TmsBackground
        ld      a, TmsTransparent
        call    TmsTextColor
        ld      de, 0                   ; clear entire VRAM
        ld      bc, 4000h
        ld      a, 0
        call    TmsFill
        ret

; initialize for multicolor mode 
TmsMulticolor:
        call    TmsReset
        ld      de, (TmsNameAddr)
        call    TmsWriteAddr
        ld      d, 6                    ; name table has 6 sections
        ld      e, 0                    ; lines in first section start at 0
TmsSectionLoop:
        ld      c, 4                    ; each section has 4 identical lines
TmsLineLoop:
        ld      b, 32                   ; each line is 32 bytes long
        ld      a, e                    ; same starting value for each line in section
TmsByteLoop: 
        call    TmsRamOut
        inc     a                       ; byte value
        djnz    TmsByteLoop
        dec     c                       ; line counter
        jp      nz, TmsLineLoop
        ld      e, a                    ; next starting value = current + 32
        dec     d                       ; section counter
        jp      nz, TmsSectionLoop
        ld      hl, TmsMulticolorFlags
        ld      (TmsMode), hl
        jp      TmsInitNonBitmap

; initialize for tiled graphics
TmsTile:
        call    TmsReset
        ld      hl, TmsTileFlags
        ld      (TmsMode), hl
        jp      TmsInitNonBitmap

; initialize for text mode
;       HL = address of font to load
TmsTextMode:
        push    hl
        call    TmsReset
        pop     hl
        ld      de, (TmsPatternAddr)    ; load font from address in hl
        ld      bc, TmsTextPatternLen
        call    TmsWrite
        ld      hl, TmsTextFlags
        ld      (TmsMode), hl
        ; fallthrough to TmsInitNonBitmap

; non-bitmap color and pattern table configuration
TmsInitNonBitmap:
        ; set up color table address (register = address / 400H)
        ld      a, (TmsColorAddr)
        and     0c0h
        ld      (TmsColorAddr), a
        ld      d, a
        ld      a, (TmsColorAddr+1)
        and     3fh
        ld      (TmsColorAddr+1), a
        rl      d
        rla
        rl      d
        rla
        ld      e, TmsColorTableReg
        call    TmsSetReg

        ; set up pattern table address (register = address / 800H)
        xor     a
        ld      (TmsPatternAddr), a
        ld      a, (TmsPatternAddr+1) 
        and     38h
        ld      (TmsPatternAddr+1), a
        rrca
        rrca
        rrca
        ld      e, TmsPatternReg
        call    TmsSetReg
        jp      TmsInitCommon

; initialize for bitmapped graphics
TmsBitmap:
        call    TmsReset
        ld      de, (TmsNameAddr)       ; initialize name table with 3 sets
        call    TmsWriteAddr            ; of 256 bytes ranging from 00-FF
        ld      b, 3
        xor     a
TmsBitmapLoop:
        call    TmsRamOut
        inc     a
        jp      nz, TmsBitmapLoop
        djnz    TmsBitmapLoop
        ld      hl, TmsBitmapFlags
        ld      (TmsMode), hl

        ; set up color table at 0H (register = 7FH) or 2000H (register = 0FFH)
        xor     a
        ld      (TmsColorAddr), a
        ld      (TmsPatternAddr), a
        ld      a, (TmsColorAddr+1)
        and     20h
        ld      (TmsColorAddr+1), a
        ld      a, 0ffh                 ; color table at 2000H
        jp      nz, TmsColorTableHigh
        ld      a, 7fh                  ; color table at 0H
TmsColorTableHigh:
        ld      e, TmsColorTableReg
        call    TmsSetReg

        ; set up pattern table at 0H (register = 3) or 2000H (register = 7)
        ld      a, (TmsPatternAddr+1)
        and     20h
        ld      (TmsPatternAddr+1), a
        ld      a, 7                    ; pattern table at 2000H
        jp      nz, TmsPatternTableHigh
        ld      a, 3                    ; pattern table at 0H
TmsPatternTableHigh:
        ld      e, TmsPatternReg
        call    TmsSetReg
        ; fall through to TmsInitCommon


; common initialization for all modes
TmsInitCommon:
        ; set up name table address (register = address / 400H)
        xor     a
        ld      (TmsNameAddr), a
        ld      a, (TmsNameAddr+1)
        and     3ch
        ld      (TmsNameAddr+1), a
        rrca
        rrca
        ld      e, TmsNameReg
        call    TmsSetReg

        ; set up sprite pattern table address (register = address / 80H)
        ld      a, (TmsSpriteAttrAddr)
        and     80h
        ld      (TmsSpriteAttrAddr), a
        ld      d, a
        ld      a, (TmsSpriteAttrAddr+1)
        and     7fh
        rl      d
        rla
        ld      e, TmsSpriteAttrReg
        call    TmsSetReg

        ; set up sprite attribute table address (register = address / 800H)
        xor     a                       
        ld      (TmsSpritePatternAddr), a
        ld      a, (TmsSpritePatternAddr+1)
        and     38h
        ld      (TmsSpritePatternAddr+1), a
        rrca
        rrca
        rrca
        ld      e, TmsSpritePatternReg
        call    TmsSetReg

        ; set up control registers
        ld      e, TmsCtrl1Reg
        ld      a, (TmsMode)
        call    TmsSetReg
        ld      a, (TmsMode+1)
        ld      e, TmsCtrl0Reg
        jp      TmsSetReg

; ---------------------------------------------------------------------------
; memory access routines

; set the next address of vram to write
;       DE = address
TmsWriteAddr:
        ld      a, e                    ; send lsb
        call    TmsRegOut
        ld      a, d                    ; mask off msb to max of 16KB
        and     3fh
        or      TmsWriteBit             ; indicate that this is a write
        call    TmsRegOut
        ret

; set the next address of vram to read
;       DE = address
TmsReadAddr:
        ld      a, e                    ; send lsb
        call    TmsRegOut
        ld      a, d                    ; mask off msb to max of 16KB
        and     3Fh
        call    TmsRegOut
        ret

; copy bytes from ram to vram
;       HL = ram source address
;       DE = vram destination address
;       BC = byte count
TmsWrite:
        call    TmsWriteAddr
TmsWriteLoop:
        ld      a, (hl)
        call    TmsRamOut
        inc     hl
        dec     bc
        ld      a, b
        or      c
        jp      nz, TmsWriteLoop
        ret

; fill a section of memory with a single value
;       A = value to fill
;       DE = vram destination address
;       BC = byte count
TmsFill:
        push    af
        call    TmsWriteAddr
        pop     af
TmsFillLoop:
        call    TmsRamOut
        dec     c
        jp      nz, TmsFillLoop
        djnz    TmsFillLoop
        ret

; ---------------------------------------------------------------------------
; text routines
; set text color
;       A = requested color
TmsTextColor:
        add     a, a                    ; text color into high nybble
        add     a, a
        add     a, a
        add     a, a
        ld      b, a                    ; save for later
        ld      a, (TmsScreenColors)    ; get current colors
        and     0fh                     ; mask off old text color
        or      b                       ; set new text color
        ld      (TmsScreenColors), a
        ld      e, TmsColorReg
        jp      TmsSetReg               ; save it back

; set the address to place text at X/Y coordinate
;       A = X
;       E = Y
TmsTextPos:
        ld      d, 0
        ld      hl, 0
        add     hl, de                  ; Y x 1
        add     hl, hl                  ; Y x 2
        add     hl, hl                  ; Y x 4
        add     hl, de                  ; Y x 5
        add     hl, hl                  ; Y x 10
        add     hl, hl                  ; Y x 20
        add     hl, hl                  ; Y x 40
        ld      e, a
        add     hl, de                  ; add X for final address
        ld      de, (TmsNameAddr)       ; add name table base address
        add     hl, de
        ex      de, hl
        jp      TmsWriteAddr

; set the address to place a tile at X/Y coordinate
;       A = X
;       E = Y
TmsTilePos:
        ld      d, 0
        ld      hl, 0
        add     hl, de                  ; Y x 1
        add     hl, hl                  ; Y x 2
        add     hl, hl                  ; Y x 4
        add     hl, hl                  ; Y x 8
        add     hl, hl                  ; Y x 16
        add     hl, hl                  ; Y x 32
        ld      e, a
        add     hl, de                  ; add X for final address
        ld      de, (TmsNameAddr)       ; add name table base address
        add     hl, de
        ex      de, hl
        jp      TmsWriteAddr 

; set the address to place a tile at X/Y coordinate into the framebuffer
;       A = X
;       E = Y
;       C = Char
TmsCharAtLocationBuffer:
        ld      d, 0
        ld      hl, 0
        sla     e
        sla     e
        sla     e
        sla     e
        sla     e
        ld      l,e
        ld      e, a
        add     hl, de                  ; add X for final address
        ld      de, FrameBuf            ; add name table base address
        add     hl, de                  ; HL is the address inside the frame buffer
        ld      (hl),c                  ; store the char into the buffer at the address pointed to by HL.
        ret

; Flush the framebuffer to the screen by doing a copy.
TmsFlushFrameBuffer:
        ld      hl,FrameBuf
        ld      de,(TmsNameAddr)
        ld      bc,0x300
        jp      TmsWrite

; copy a null-terminated string to VRAM
;       HL = ram source address
TmsStrOut:
        ld      a, (hl)
        cp      0                       ; return when NULL is encountered
        ret     z
        call    TmsRamOut
        inc     hl
        jp      TmsStrOut

; repeat a character a certain number of times
;       A = character to output
;       B = count
TmsRepeat:
        call    TmsRamOut
        djnz    TmsRepeat
        ret

; output a character
;       A = character to output
TmsChrOut:      equ TmsRamOut

; ---------------------------------------------------------------------------
; bitmap routines

TmsClearPixel:  equ 0A02Fh              ; cpl, and b
TmsSetPixel:    equ 0B0h                ; nop, or b

; set operation for TmsPlotPixel to perform
;       HL = pixel operation (TmsClearPixel, TmsSetPixel)
TmsPixelOp:
        ld      (TmsPixelOpPlaceHolder), hl
        ret

; set or clear pixel at X, Y position
;       B = Y position
;       C = X position
TmsPlotPixel:
        ld      a, b                    ; bail out if Y coord > 191
        cp      192
        ret     nc
        call    TmsXYAddr               ; get address in DE for X/Y coord in BC
        ld      a, c                    ; get lower 3 bits of X coord
        and     7
        ld      b, 0
        ld      c, a
        ld      hl, TmsMaskLookup       ; address of mask in table
        add     hl, bc
        ld      b, (hl)                 ; save mask in B
        ld      hl, (TmsPatternAddr)    ; get base address for pattern table
        add     hl, de
        ex      de, hl
        call    TmsReadAddr             ; set read within pattern table
        call    TmsRamIn
TmsPixelOpPlaceHolder:
        or      b                       ; mask bit in previous byte
        nop                             ; place holder for 2 byte mask operation
        push    af
        call    TmsWriteAddr            ; set write address within pattern table
        pop     af
        jp      TmsRamOut

TmsMaskLookup:
        defb 80h, 40h, 20h, 10h, 8h, 4h, 2h, 1h

; set the color for a block of pixels in bitmap mode
;       B = Y position
;       C = X position
;       A = foreground/background color to set
TmsPixelColor:
        push    af
        ld      a, b                    ; bail out if Y coord > 191
        cp      192
        ret     nc
        call    TmsXYAddr               ; get address in DE for X/Y coord in BC
        ld      hl, (TmsColorAddr)      ; add the color table base address
        add     hl, de
        ex      de, hl
        call    TmsWriteAddr            ; set write address within color table
        pop     af
        jp      TmsRamOut

; calculate address byte containing X/Y coordinate
;       B = Y position
;       C = X position
;       returns address in DE
TmsXYAddr:
        ld      a, b                    ; d = (y / 8)
        rrca
        rrca
        rrca
        and     1fh
        ld      d, a

        ld      a, c                    ; e = (x & f8)
        and     0f8h
        ld      e, a

        ld      a, b                    ; e += (y & 7)
        and     7
        or      e
        ld      e, a
        ret

FrameBuf:
        defs 0x300                              ; define a 0x300 buffer for the tiles. Defaults to 0