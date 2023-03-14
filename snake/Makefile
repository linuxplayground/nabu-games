include ../Makefile
.PHONY: all
all: cpm nabu disk copy pub

cpm: 
	$(ZCC) $(CFLAGS) -o$(BUILDDIR)/SNAKE.COM main.c

nabu:
	$(ZCC) $(NABUCFLAGS) -o$(BUILDDIR)/000002.nabu main.c