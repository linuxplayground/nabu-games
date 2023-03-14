BASEDIR=/home/davidlatham/dev/nabu/nabu-games

ZCC=$(HOME)/dev/retro/z88dk-build/z88dk/bin/zcc
INCLUDES = lib/
CFLAGS = -I$(INCLUDES) +cpm -DBIN_TYPE=BIN_CPM --list -compiler=sdcc -create-app -O3 --opt-code-speed
NABUCFLAGS = +nabu -vn -DBIN_TYPE=NABU --list -m -create-app -compiler=sdcc

BUILDDIR = $(BASEDIR)/build
DISKDIR = $(BASEDIR)/disk
COMFILES = $(shell find $(BUILDDIR) -name '*.COM')
STOREDIR = "/mnt/c/dev/nabu/Nabu Internet Adapter/Store/"

PROJECTS = 'snake' 'tetris'
MKDIR = mkdir -pv

export PATH := $(HOME)/dev/retro/z88dk-build/z88dk/bin/:$(PATH)
export ZCCCFG := $(HOME)/dev/retro/z88dk-build/z88dk/lib/config

# Override any of the above settings in your own make.local file
-include make.local

.PHONY: all cpm nabu disk copy pub clean
all: clean $(PROJECTS) disk copy pub

$(PROJECTS):
	$(MKDIR) $(BUILDDIR)/$@
	$(ZCC) $(CFLAGS) -o$(BUILDDIR)/$@/$(shell echo $@ | tr "a-z" "A-Z").COM $@/main.c
	$(ZCC) $(NABUCFLAGS) -o$(BUILDDIR)/$@/000001.NABU $@/main.c

cpm:
	$(MKDIR) $(BUILDDIR)/$(PROJECT)
	$(ZCC) $(CFLAGS) -o$(BUILDDIR)/$(PROJECT)/$(shell echo $(PROJECT) | tr "a-z" "A-Z").COM $(SOURCE)

nabu:
	$(MKDIR) $(BUILDDIR)$(PROJECT)
	$(ZCC) $(NABUCFLAGS) -o$(BUILDDIR)/$(PROJECT)/000001.NABU main.c

disk:
	mkfs.cpm -f naburn $(DISKDIR)/c.dsk

copy: 
	cpmcp -f naburn $(DISKDIR)/c.dsk $(COMFILES) 0:

pub: $(DISKDIR)/c.dsk
	cp $? $(STOREDIR)

clean:
	rm -frv $(BUILDDIR)/*
	rm -frv $(DISKDIR)/*

