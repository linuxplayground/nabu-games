# Override any of the above settings in your own make.local file

TOP=../../..

# case here has to match the directory name
PROGS=fileio.com
NABUS=000001.nabu

all:: $(PROGS) $(NABUS)

include ../Make.rules

SRC_DIR=$(TOP)/fileio

%.com: $(SRC_DIR)/fileio.c
	$(ZCC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

000001.nabu: $(SRC_DIR)/fileio.c
	$(ZCC) $(NABUCFLAGS) $(CPPFLAGS) -o $@ $^

world:: clean all

clean::
	rm -f *.com *.lis *.COM *.NABU *.nabu *.map

