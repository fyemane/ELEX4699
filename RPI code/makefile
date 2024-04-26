# The compiler to use is for C++
CC=g++
# The compiler options are (all warnings)
CFLAGS=-Wall `pkg-config --cflags opencv4` -std=c++11 -c
# The linker options are (all warnings)
LDFLAGS=-Wall `pkg-config --libs opencv4` -std=c++11 -lpigpio -lpthread

all: main

main: main.o CControlPi.o CBase4618.o CSketch.o CPong.o cvui.h
	$(CC) $(LDFLAGS) main.o CControlPi.o CBase4618.o CSketch.o CPong.o -o hello

main.o: main.cpp cvui.h
	$(CC) $(CFLAGS) main.cpp

CControlPi.o: CControlPi.cpp cvui.h
	$(CC) $(CFLAGS) CControlPi.cpp

CBase4618.o: CBase4618.cpp cvui.h
	$(CC) $(CFLAGS) CBase4618.cpp

CSketch.o: CSketch.cpp cvui.h
	$(CC) $(CFLAGS) CSketch.cpp

CPong.o: CPong.cpp
	$(CC) $(CFLAGS) CPong.cpp

clean:
	rm *o main
