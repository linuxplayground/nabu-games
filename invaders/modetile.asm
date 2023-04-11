; Tile Mode on TMS9918A example program
; by David Latham - 2023
;
; Based on TMS9918A graphics subroutines
; Copyright 2018-2020 J.B. Langston
;

        org     100h

.bufx:  defb 0x00
.bufy:  defb 0x00
.bufc:  defb 0x00

main:
        ld      (OldSP),sp                      ; save old Stack poitner
        ld      sp, Stack                       ; set up Stack

        call    TmsSetWait                      ; set VDP wait loop based on clock multiple

        call    TmsProbe                        ; find what port TMS9918A listens on
        jp      z, NoTms

        call    TmsTile                         ; Set up bitmap graphics mode.

        ; load font.
        ld      hl,TmsFont
        ld      de,(TmsPatternAddr)
        ld      bc,TmsFontLen
        call    TmsWrite

        ; Set background to dark yellow
        ld      a,TmsDarkYellow
        call    TmsBackground

        ; load all colours to Dark Red on Black
        ld      a,(TmsDarkBlue<<4)|TmsBlack
        ld      de,(TmsColorAddr)
        ld      bc,0x300
        call    TmsFill

.start:
        ; Load all name cells to be 00
        or      a
        ld      (.bufc),a

.loop:
        call    keypress
        jp      nz,Exit
        ld      a,(.bufc)
        ld      de,(TmsNameAddr)
        ld      bc,0x300
        call    TmsFill
        ld      a,(.bufc)
        inc     a
        cp      0xFF
        jp      z,.start
        ld      (.bufc),a
        jp      .loop
Exit:
        call    getchar
        ld      sp, (OldSP)
        rst     0

NoTmsMessage:
        defb    "TMS9918A not found, aborting!$"
NoTms:  ld      de, NoTmsMessage
        call    strout
        jp      Exit
TmsFont:
        include "TmsFont.asm"
TmsFontLen:     equ $-TmsFont

        include "tms.asm"                       ; TMS graphics routines
        include "utility.asm"

OldSP:
        defw 0
        defs 64
Stack: