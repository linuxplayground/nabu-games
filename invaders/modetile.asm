; Tile Mode on TMS9918A example program
; by David Latham - 2023
;
; Based on TMS9918A graphics subroutines
; Copyright 2018-2020 J.B. Langston
;

        org     100h
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
        ld      a,(TmsDarkRed<<4)|TmsBlack
        ld      de,(TmsColorAddr)
        ld      bc,0x300
        call    TmsFill

        ; Load all name cells to be 00
        ld      a,0x00
        ld      de,(TmsNameAddr)
        ld      bc,0x300
        call    TmsFill

        ; test set cursor
        ld      a,16
        ld      e,12
        call    TmsTilePos
        ; test write a char.
        ld      a,'A'
        call    TmsChrOut
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