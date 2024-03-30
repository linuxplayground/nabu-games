
TOP=../..

# Name each subdir to build before making the filesystem
SUBDIRS=\
	tetris \
	snake \
	invaders \
	imagui

.PHONY: c.dsk all store

# make 'all' be the default build target...
all::

include Make.rules

# ...but don't try to make the disk image until after the subdirs are done!
all:: c.dsk

# make sure all subdirs are built before we make the disk image
c.dsk:: $(ALL_DIRS)
	rm -f $@
	LC_CTYPE=C tr '\0' '\345' < /dev/zero | dd of=$@ bs=8M count=1 conv=fsync iflag=fullblock
	cpmcp -f $(DISKDEF) $@ */*.com 0:
	cpmls -f $(DISKDEF) $@

NDSK_B.img:: $(ALL_DIRS)
	rm -f $@
	LC_CTYPE=C tr '\0' '\345' < /dev/zero | dd of=$@ bs=8M count=1 conv=fsync iflag=fullblock
	cpmcp -f nshd8 $@ */*.com 0:
	cpmls -f nshd8 $@

store: c.dsk
	cp $^ $(STOREDIR)

ishkur: NDSK_B.img
	cp $^ $(ISHKURDIR)

clean::
	rm -f c.dsk

release: $(ALL_DIRS)
	@if [ -e $(GIT_TAG) ]; then echo "\n\n** ERROR: you have not tagged this commit!\n\n"; exit 1; fi
	tar -c -z -v -f  nabu-games-$(GIT_TAG).tgz snake/SNAKE.COM snake/000001.NABU tetris/TETRIS.COM tetris/000001.NABU invaders/INVADERS.COM invaders/000001.NABU
	zip  -r -v  nabu-games-$(GIT_TAG).zip snake/SNAKE.COM snake/000001.NABU tetris/TETRIS.COM tetris/000001.NABU invaders/INVADERS.COM invaders/000001.NABU
