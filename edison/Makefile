# memos: $@ file name of the target of the rule
#	 $^ all dependencies of this rule
#	 $< the first dependency
#	 .PRECIOUS targets marked with this are not deleted when make is killed

CXX=g++
CFLAGS=-g -Wall -lmraa -lupm-i2clcd -I/usr/include/upm -I/usr/include/mraa
COPTS=#`sdl-config --cflags`
LOPTS=#`sdl-config --libs` -pthread 

MAIN_BINARIES=$(basename $(wildcard *Main.cpp))
TEST_BINARIES=$(basename $(wildcard *Test.cpp))
HEADER=$(wildcard *.h)
OBJECTS=

.PRECIOUS: %.o

all: compile test checkstyle

compile: $(MAIN_BINARIES) $(TEST_BINARIES)

test: $(TEST_BINARIES)
	for T in $(TEST_BINARIES); do ./$$T; done

checkstyle:
	python2 cpplint.py --filter=-runtime/references,-whitespace/comments,-readability/streams --linelength=200 *.cpp

clean:
	rm -f *.o
	rm -f *~
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)

%Main: %Main.o $(OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LOPTS)

%Test: %Test.o $(OBJECTS)
	$(CXX) $(CFLAGS) $(LOPTS) -o $@ $^ -lgtest -lgtest_main

%.o: %.cpp $(HEADER)
	$(CXX) $(CFLAGS) $(COPTS) -c $<
