#include "pdp7_cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    PDP7 cpu;
    initialize_pdp7(&cpu);

    bool debug = false;
    bool single_instruction = false;

    const char *program_file = "data/program.dat";
    const char *memory_file = "data/memory.dat";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            single_instruction = true;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            program_file = argv[++i];
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            memory_file = argv[++i];
        } else {
            fprintf(stderr, "Usage: %s [-s] [-d] [-p <program file>] [-m <memory file>]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Load instruction memory from file
    load_memory_from_file(&cpu, program_file, INSTRUCTION_START);

    // Load data memory from file
    load_memory_from_file(&cpu, memory_file, 0);

    // Main emulation loop
    while (cpu.running) {
        perform_cycle(&cpu);

        if (debug) {
            print_cpu_state(&cpu);
        }

        if (single_instruction) {
            print_cpu_state(&cpu);
            printf("Press any key to continue, or ESC to exit\n");
            char c;
            scanf("%c", &c);
            if (c == 27) {
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}