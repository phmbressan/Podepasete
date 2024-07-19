CC = gcc
CFLAGS = -Wall -Wextra -O0
PROG = program.dat
MEM = memory.dat

clear: main.out
	rm main.out

build: main.c
	$(CC) $(CFLAGS) -o main.out main.c

run: build
	./main.out -p $(PROG) -m $(MEM)

debug: build
	./main.out -d -p $(PROG) -m $(MEM)

.PHONY: build run