# Compiler
CC = gcc
ARM_CC = arm-none-eabi-gcc

# Compiler flags
CFLAGS = -I./src -I./tests -I/usr/include/SDL2
CFLAGS += -W
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wformat
CFLAGS += -Wmissing-braces
CFLAGS += -Wno-cast-align
CFLAGS += -Wparentheses
CFLAGS += -Wshadow
CFLAGS += -Wno-sign-compare
CFLAGS += -Wswitch
CFLAGS += -Wuninitialized
CFLAGS += -Wunknown-pragmas
CFLAGS += -Wunused-function
CFLAGS += -Wunused-label
CFLAGS += -Wunused-parameter
CFLAGS += -Wunused-value
CFLAGS += -Wunused-variable
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wmissing-declarations
LDFLAGS = -lSDL2

# Directories
SRCDIR = src
TESTDIR = tests
FIXTUREDIR = $(TESTDIR)/fixtures
UTILDIR = $(TESTDIR)/utils
BUILDDIR = build
BINDIR = bin
OBJDIR = $(BUILDDIR)/obj
ASMDIR = $(BUILDDIR)/assembly
TESTOBJDIR = $(OBJDIR)/test

# Source files
SRC = $(wildcard $(SRCDIR)/*.c)
TEST_SRC = $(wildcard $(TESTDIR)/unit/*.c)
FIXTURE_SRC = $(wildcard $(FIXTUREDIR)/*.c)
UTIL_SRC = $(wildcard $(UTILDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
TEST_OBJ = $(patsubst $(TESTDIR)/unit/%.c, $(TESTOBJDIR)/%.o, $(TEST_SRC)) \
           $(patsubst $(FIXTUREDIR)/%.c, $(TESTOBJDIR)/%.o, $(FIXTURE_SRC)) \
           $(patsubst $(UTILDIR)/%.c, $(TESTOBJDIR)/%.o, $(UTIL_SRC))
ASM = $(patsubst $(SRCDIR)/%.c, $(ASMDIR)/%.s, $(SRC))

# Executable names
TARGET = $(BINDIR)/pdp7_emulator
TEST_TARGET = $(BINDIR)/pdp7_tests

# Default data files
PROG_FILE = data/program.dat
MEM_FILE = data/memory.dat

# Rules
all: $(TARGET) $(ASM) $(TEST_TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ASMDIR)/%.s: $(SRCDIR)/%.c
	@mkdir -p $(ASMDIR)
	$(CC) $(CFLAGS) -S $< -o $@

clean:
	rm -rf $(BUILDDIR)

debug: $(TARGET)
	@$(TARGET) -p $(PROG_FILE) -m $(MEM_FILE) -d -t -s

# Test rules
$(TEST_TARGET): $(TARGET) $(TEST_OBJ) 
	@mkdir -p $(BINDIR)
	$(CC) $(TEST_OBJ) build/obj/pdp7_cpu.o build/obj/teleprinter.o -o $(TEST_TARGET) $(LDFLAGS)

$(TESTOBJDIR)/%.o: $(TESTDIR)/unit/%.c
	@mkdir -p $(TESTOBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTOBJDIR)/%.o: $(FIXTUREDIR)/%.c
	@mkdir -p $(TESTOBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTOBJDIR)/%.o: $(UTILDIR)/%.c
	@mkdir -p $(TESTOBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

.PHONY: all clean debug test
