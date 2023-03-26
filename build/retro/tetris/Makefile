# Override any of the above settings in your own make.local file

TOP=../../..

# case here has to match the directory name
PROGS=tetris.com

all:: $(PROGS)

include ../Make.rules

SRC_DIR=$(TOP)/tetris

%.com: $(SRC_DIR)/tetris.c
	$(ZCC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

world:: clean all

clean::
	rm -f *.com *.lis *.COM




#
#all: $(PROJECTS) disk copy pub
#
#$(PROJECTS):
#	$(MKDIR) $(BUILDDIR)/$@
#	$(ZCC) $(CFLAGS) -o$(BUILDDIR)/$@/$(shell echo $@ | tr "a-z" "A-Z").COM $@/main.c
#	$(ZCC) $(NABUCFLAGS) -o$(BUILDDIR)/$@/000001.NABU $@/main.c
#
#cpm:
#
#nabu:
#	$(MKDIR) $(BUILDDIR)$(PROJECT)
#	$(ZCC) $(NABUCFLAGS) -o$(BUILDDIR)/$(PROJECT)/000001.NABU main.c
#
#disk:
#	dd if=/dev/zero of=$(DISKDIR)/C.DSK bs=512 count=16384 
#	mkfs.cpm -f naburn $(DISKDIR)/C.DSK
#
#copy: 
#	cpmcp -f naburn $(DISKDIR)/C.DSK $(COMFILES) 0:
#
#pub: $(DISKDIR)/C.DSK
#	cp $? $(STOREDIR)
#
#clean:
#	rm -frv $(BUILDDIR)/*
#	rm -frv $(DISKDIR)/*
#
#release:
#	cd $(BUILDDIR); tar -c -z -v -f  nabu-games-$(GIT_TAG).tgz */*.COM */*.NABU
#	cd $(BUILDDIR); zip  -r -v  nabu-games-$(GIT_TAG).zip */*.COM */*.NABU
#
