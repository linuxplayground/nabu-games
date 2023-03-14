include ../Makefile
.PHONY: all
all: cpm nabu disk copy pub

cpm: 
	$(ZCC) $(CFLAGS) -o$(BUILDDIR)/TETRIS.COM main.c

nabu:
	$(ZCC) $(NABUCFLAGS) -o$(BUILDDIR)/000001.nabu main.c