ZCC=$(HOME)/dev/retro/z88dk-build/z88dk/bin/zcc
INCLUDES = lib/
CFLAGS = -I$(INCLUDES) +cpm --list -compiler=sdcc -create-app -O3 --opt-code-speed
BUILDDIR = ../build
DISKDIR = ../disk
COMFILES = ../build/*.COM
STOREDIR = "/mnt/c/dev/nabu/Nabu Internet Adapter/Store/"

export PATH := $(HOME)/dev/retro/z88dk-build/z88dk/bin/:$(PATH)
export ZCCCFG := $(HOME)/dev/retro/z88dk-build/z88dk/lib/config

disk: $(COMFILES)
	mkfs.cpm -f naburn    $(DISKDIR)/c.dsk

copy: $(COMFILES)
	cpmcp    -f naburn    $(DISKDIR)/c.dsk $? 0:

pub: ../disk/c.dsk
	cp $? $(STOREDIR)
clean:
	rm -fv $(BUILDDIR)/*
	rm -fv $(DISKDIR)/*

