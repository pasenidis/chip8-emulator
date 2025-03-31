CC = gcc
CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

all: chip8

chip8: main.o chip8.o
	$(CC) -o chip8 main.o chip8.o $(LDFLAGS)

main.o: main.c chip8.h
	$(CC) -c main.c $(CFLAGS)

chip8.o: chip8.c chip8.h
	$(CC) -c chip8.c $(CFLAGS)

clean:
	rm -f chip8 *.o
