
TOP=../..

# Name each subdir to build before making the filesystem
SUBDIRS=\
	tetris \
	snake \
	framebuf

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

store: c.dsk
	cp $^ $(STOREDIR)

clean::
	rm -f c.dsk