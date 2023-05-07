; NABU
if is_nabu
io_tmsdata:             equ 0xa0      ; NABU
io_tmslatch:            equ 0xa1      ; NABU
io_keyboard:            equ 0x90      ; NABU

joy_map_left:           equ %00000001
joy_map_down:           equ %00000010
joy_map_right:          equ %00000100
joy_map_up:             equ %00001000
joy_map_button:         equ %00010000

else

; RETRO
io_tmsdata:             equ 0x80    
io_tmslatch:            equ 0x81   
joy0:                   equ 0xa8      
joy1:                   equ 0xa9       

joy_map_left:           equ %00000100
joy_map_right:          equ %00100000
joy_map_up:             equ %10000000
joy_map_down:           equ %01000000
joy_map_button:         equ %00000001

endif