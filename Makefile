CC = gcc
ARM = arm-none-eabi-gcc
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wunused -Wshadow -Wunreachable-code -Wconversion -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum
SRC = src
PROG = data/program.dat
MEM = data/memory.dat
OPT = -O3

clear: $(SRC)/main.out
	rm src/main.out

build: $(SRC)/main.c
	$(CC) $(CFLAGS) -o $(SRC)/main.out $(SRC)/main.c

build-release: $(SRC)/main.c
	$(CC) $(CFLAGS) -o $(SRC)/main.out $(SRC)/main.c $(OPT)

assemble: $(SRC)/main.c
	$(CC) $(CFLAGS) -S -o $(SRC)/main.s $(SRC)/main.c

arm-assemble: $(SRC)/main.c
	$(ARM) $(CFLAGS) -S -o $(SRC)/main-arm.s $(SRC)/main.c

run: build
	./$(SRC)/main.out -p $(PROG) -m $(MEM)

debug: build
	./$(SRC)/main.out -d -p $(PROG) -m $(MEM)

run-release: build-release
	./$(SRC)/main.out -p $(PROG) -m $(MEM)

.PHONY: build run