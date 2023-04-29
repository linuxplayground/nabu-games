bdos:           equ 0x0005
con_out:	equ 0x02
con_status:     equ 0x0b

check_cpm_key:
        ld      c,con_status
        call    bdos
        or      a
        jp      nz,cpm_terminate
        ret

puts:
	ld	e,a
	ld	c,con_out
	call	bdos
        ret

cpm_terminate:
        jp      0

; Fills memory with a single value.
; a = value to fill with
; hl = start address
; bc = count / size to fill.
fillmem:
	ld 	(hl),a
	ld 	e,l
	ld 	d,h
	inc 	de
	dec 	bc
	ldir
	ret

;#############################################################################
; Print the value in A in hex
; Clobbers C
;#############################################################################
hexdump_a:
	push	af
	srl	a
	srl	a
	srl	a
	srl	a
	call	.hexdump_nib
	pop	af
	push	af
	and	0x0f
	call	.hexdump_nib
	pop	af
	ret

.hexdump_nib:
	add	'0'
	cp	'9'+1
	jp	m,.hexdump_num
	add	'A'-'9'-1
.hexdump_num:
	jp	puts