# Override any of the above settings in your own make.local file

TOP=../../..

# case here has to match the directory name
PROGS=snake.com
NABUS=00001.nabu

all:: $(PROGS) $(NABUS)

include ../Make.rules

SRC_DIR=$(TOP)/snake

%.com: $(SRC_DIR)/snake.c
	$(ZCC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

00001.nabu: $(SRC_DIR)/snake.c
	$(ZCC) $(NABUCFLAGS) $(CPPFLAGS) -o $@ $^

world:: clean all

clean::
	rm -f *.com *.lis *.COM *.NABU *.nabu *.map

