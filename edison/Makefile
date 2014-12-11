# memos: $@ file name of the target of the rule
#	 $^ all dependencies of this rule
#	 $< the first dependency
#	 .PRECIOUS targets marked with this are not deleted when make is killed

CXX=g++
CFLAGS=-g -Wall -lmraa -lupm-i2clcd -I/usr/include/upm -I/usr/include/mraa
COPTS=#`sdl-config --cflags`
LOPTS=#`sdl-config --libs` -pthread 

SOURCES=edison_serial.cpp lasertag.cpp
MAIN_BINARIES=$(basename $(wildcard *Main.cpp))
HEADER=$(wildcard *.h)
OBJECTS=$(SOURCES:.cpp=.o)

.PRECIOUS: %.o

all: compile checkstyle

compile: $(SOURCES) $(MAIN_BINARIES)

checkstyle:
	python2 cpplint.py --filter=-runtime/references,-whitespace/comments,-readability/streams --linelength=200 *.cpp

clean:
	rm -f *.o
	rm -f *~
	rm -f $(MAIN_BINARIES)

%Main: %Main.o $(OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LOPTS)

%.o: %.cpp $(HEADER)
	$(CXX) $(CFLAGS) $(COPTS) -c $<
