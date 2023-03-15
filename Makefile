# Override any of the above settings in your own make.local file
SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)Make.default
-include $(SELF_DIR)Make.local

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

