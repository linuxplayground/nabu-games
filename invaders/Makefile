.PHONY: all clean

PROGS= \
	ascii \
	modetile

all: clean store

$(PROGS:%=%.com):
	z80asm -I. -o $@ --list=$(@:%.com=%.lst) --label=$(@:%.com=%.sym) $(@:%.com=%.asm)

c.dsk: $(PROGS:%=%.com)
	rm -f $@
	LC_CTYPE=C tr '\0' '\345' < /dev/zero | dd of=$@ bs=8M count=1 conv=fsync iflag=fullblock
	cpmcp -f naburn $@ *.com 0:
	cpmls -f naburn $@

store: c.dsk
	cp $^ "/mnt/c/dev/nabu/Nabu Internet Adapter/Store/"

clean:
	rm -f c.dsk *.com *.lst *.sym