        include 'io.asm'

bdos:           equ 0x0005
con_out:	equ 0x02
conio:		equ 0x06
con_status:     equ 0x0b

joy_status:		ds  4
kbd_buffer:		ds  0xff
kbd_buffer_read_pos:	db 0
kbd_buffer_write_pos:	db 0

if is_nabu
lastKeyIntVal:	db 0

; this shit right here only tracks the joystick status.
; it does nothing for keyboard stuff.
; taken from C Listing file generated by Z88DK when compiling stuff with NABU-LIB
;
isrKeyboard:
        push	bc
        push	de
        push	hl
        push	af
        push	iy
        
        ld	iy,0
        in	a,(io_keyboard)
        ld	c,a
        sub	a,0x80
        jr	c,isrKeyboard_1
        ld	a,0x83
        sub	a,c
        jr	c,isrKeyboard_1
        ld	iy,lastKeyIntVal
        ld	(iy+0),c
        jr	isrKeyboard_exit
isrKeyboard_1:
        ld	a,c
        sub	0x90
        jr	c,isrKeyboard_2
        ld	a,0x95
        sub	a,c
        jr	nc,isrKeyboard_exit
isrKeyboard_2:
        ld	a,(lastKeyIntVal+0)
        cp	0x80
        jr	z,isrKeyboard_3
        cp	0x81
        jr	z,isrKeyboard_4
        cp	0x82
        jr	z,isrKeyboard_5
        cp	0x83
        jr	z,isrKeyboard_6
        jr	isrKeyboard_7
isrKeyboard_3:
        ld	iy,lastKeyIntVal
        ld	(iy+0),0x00
        ld	hl,joy_status
        ld	(hl),c
        jr	isrKeyboard_exit
isrKeyboard_4:
        ld	iy,lastKeyIntVal
        ld	(iy+0),0x00
        ld	hl,joy_status+1
        ld	(hl),c
        jr	isrKeyboard_exit
isrKeyboard_5:
        ld	iy,lastKeyIntVal
        ld	(iy+0),0x00
        ld	hl,joy_status+2
        ld	(hl),c
        jr	isrKeyboard_exit
isrKeyboard_6:
        ld	iy,lastKeyIntVal
        ld	(iy+0),0x00
        ld	hl,joy_status+3
        ld	(hl),c
        jr	isrKeyboard_exit
isrKeyboard_7:					; not a joystick. Add key press to buffer
        ld	hl,kbd_buffer+0			; increment buffer write position.
        ld	de,(kbd_buffer_write_pos)
        ld	d,0
        add	hl,de
        ld	(hl),c
        ld	a,(kbd_buffer_write_pos)
        inc	a
        ld	(kbd_buffer_write_pos),a
isrKeyboard_exit:
        pop	iy	
        pop	af
        pop	hl
        pop	de
        pop	bc
        ei
        reti

; a = 0 no keypress
; a = 1 key pressed.
isKeyPressed:
        ld	a,(kbd_buffer_write_pos)
        ld	iy,kbd_buffer_read_pos
        sub	a,(iy+0)
        jr	nz,isKeyPressed_1
        jr	isKeyPressed_2
isKeyPressed_1:				; a key was pressed - return 1 in A
        ld	a,1
        ret
isKeyPressed_2:				; no key was pressed - return 0 in A
        ld	a,0
        ret

; blocks until char is entered.
; returns char in A
getChar:
        ld	a,(kbd_buffer_write_pos)
        ld	iy,kbd_buffer_read_pos
        sub	a,(iy+0)
        jr	z,getChar		; back to getChar
        ld	bc,kbd_buffer
        ld	hl,(kbd_buffer_read_pos)
        ld	h,0x00
        add	hl,bc
        ld	c,(hl)			; c contains ascii value
        ld	a,(kbd_buffer_read_pos)
        inc	a			; increment and store buffer read pos
        ld	(kbd_buffer_read_pos),a
        ld	a,c			; copy c to a
        ret

; a = which joystick to read 0 or 1
; output is in A
getJoyStatus:
        or      a
        jr      z,.getJoy0
        ld      a,(joy_status+1)
        and     0x1f
        ret
.getJoy0:
        ld      a,(joy_status+0)
        and     0x1f
        ret

else    ; not a nabu - must be a retro

isKeyPressed:
        in      a,(0x32)        ; read sio control status byte
        and     1               ; check the rcvr ready bit
        ret

getChar:
        in    a,(0x30)          ; read sio control status byte
        ret

; get joy stick status.  Which one to get 0 or 1 is in A
; output is in A
getJoyStatus:
        or      a
        jr      z,.getJoy0
.getJoy1:
        in      a,(joy1)
        xor     0xff
        ld      (joy_status+1),a
        ret
.getJoy0:
        in      a,(joy0)
        xor     0xff
        ld      (joy_status+0),a
        ret

endif

waitForKey:
        call    isKeyPressed
        or      a
        jp      z,waitForKey
        ret

check_cpm_key:
        ld      c,con_status
        call    bdos
        or      a
        jp      nz,cpm_terminate
        ret

getk:
        ld	c,conio
        ld	e,0xff
        call	bdos
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