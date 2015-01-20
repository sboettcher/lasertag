# memos: $@ file name of the target of the rule
#	 $^ all dependencies of this rule
#	 $< the first dependency
#	 .PRECIOUS targets marked with this are not deleted when make is killed

CXX=g++
CFLAGS=-g -Wall -std=c++0x -lmraa -lupm-i2clcd -Iinclude -I/usr/include/upm -I/usr/include/mraa
COPTS=
LOPTS=-pthread

SOURCES = src/edison_serial.cpp \
					src/lasertag.cpp \
					src/TFT_22_ILI9225.cpp \
					src/DefaultFonts.cpp \
					src/tcp_client.cpp \
					src/MFRC522.cpp

MAIN_BINARIES=$(addprefix bin/,$(basename $(notdir $(wildcard src/*Main.cpp))))
OBJECTS = $(subst src/,build/,$(subst .cpp,.o,$(SOURCES)))


.PRECIOUS: build/%.o

all: compile

compile: dirs $(SOURCES) $(MAIN_BINARIES)

bin/%Main: $(OBJECTS) build/%Main.o
	$(CXX) $(CFLAGS) -o $@ $^ $(LOPTS)

build/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ $(COPTS)



dirs:
	mkdir -p bin build

checkstyle:
	python2 cpplint.py --filter=-runtime/references,-whitespace/comments,-readability/streams --linelength=200 *.cpp

.PHONY:
	clean

clean:
	rm -rf bin build
	rm -f *~
	rm -f */*~
