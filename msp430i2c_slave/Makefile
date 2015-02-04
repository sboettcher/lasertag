# MSP Makefile
# memos: $@ file name of the target of the rule
#	 $^ all dependencies of this rule
#	 $< the first dependency
#	 .PRECIOUS targets marked with this are not deleted when make is killed

CXX=msp430-gcc
CFLAGS=-mmcu=msp430g2553
COPTS= -Wall
LOPTS=

MAIN_BINARY=$(basename $(wildcard *main.c))
HEADER=$(wildcard *.h)
SOURCES=$(wildcard *.c)
OBJECTS=$(subst .c,.o,$(wildcard *.c))


.PRECIOUS: %.o

all: compile link

compile: $(SOURCES)
	$(CXX) $(CFLAGS) $(COPTS) -c $^

link: $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(MAIN_BINARY) $^ $(LOPTS)

clean:
	rm -f *.o
