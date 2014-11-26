# MSP Makefile
# memos: $@ file name of the target of the rule
#	 $^ all dependencies of this rule
#	 $< the first dependency
#	 .PRECIOUS targets marked with this are not deleted when make is killed

CXX=msp430-elf-gcc
CFLAGS=-O -Wall -mmcu=msp430g2553
COPTS=
LOPTS=

MAIN_BINARIES=$(basename $(wildcard *main.c))
HEADER=$(wildcard *.h)
SOURCES=$(wildcard *.c)


.PRECIOUS: %.o

all: compile link

compile: $(SOURCES)
	$(CXX) $(CFLAGS) $(COPTS) -c $^

link: $(MAIN_BINARIES)

$(MAIN_BINARIES):	OBJECTS = $(wildcard *.o)
$(MAIN_BINARIES):
	$(CXX) $(CFLAGS) -o $@ $(OBJECTS) $(LOPTS)

clean:
	rm -f *.o
