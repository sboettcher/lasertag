CXX = g++ -g -Wall
MAIN_BINARIES = $(basename $(wildcard *Main.cpp))
TEST_BINARIES = $(basename $(wildcard *Test.cpp))
HEADERS = $(wildcard *.h)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp %Test.cpp,$(wildcard *.cpp))))

.PRECIOUS: %.o

all: build test lint

build: $(MAIN_BINARIES) $(TEST_BINARIES)

lint:
	python ../cpplint.py *.cpp
	#python ../cpplint.py *.cpp *.h

test: $(TEST_BINARIES)
	for T in $(TEST_BINARIES); do ./$$T; done

clean:
	rm -f *.o
	rm -f *Main
	rm -f *Test
	rm -f core

%Main: %Main.o $(OBJECTS)
	$(CXX) -o $@ $^ -lncurses

%Test: %Test.o $(OBJECTS)
	$(CXX) -o $@ $^ -lgtest -lgtest_main -lpthread

%.o: %.cpp $(HEADERS)
	$(CXX) -c $<
