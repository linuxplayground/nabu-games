; NABU
if is_nabu
io_tmsdata:             equ 0xa0      ; NABU
io_tmslatch:            equ 0xa1      ; NABU
io_keyboard:            equ 0x90      ; NABU

joy_map_left:           equ 0xa1      ; NABU
joy_map_right:          equ 0xa4      ; NABU
joy_map_button:         equ 0xb0      ; NABU

else

; RETRO
io_tmsdata:             equ 0x80        ; Z80-Retro!
io_tmslatch:            equ 0x81        ; Z80-Retro!
joy0:                   equ 0xa8        ; Z80-Retro!
joy1:                   equ 0xa9        ; Z80-Retro!
joy_map_left:           equ 0xfb        ; Z80-Retro!
joy_map_right:          equ 0xdf        ; Z80-Retro!
joy_map_button:         equ 0xfe        ; Z80-Retro!

endif