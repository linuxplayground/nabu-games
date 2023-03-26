# Override any of the above settings in your own make.local file

TOP=../../..

# case here has to match the directory name
PROGS=snake.com

all:: $(PROGS)

include ../Make.rules

SRC_DIR=$(TOP)/snake

%.com: $(SRC_DIR)/snake.c
	$(ZCC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

world:: clean all

clean::
	rm -f *.com *.lis *.COM

