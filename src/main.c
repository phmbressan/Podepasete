#include "pdp7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    bool debug = false;
    bool single_instruction = false;
    bool use_teleprinter = false;

    const char *program_file = "data/program.dat";
    const char *memory_file = "data/memory.dat";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "-t") == 0) {
            use_teleprinter = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            single_instruction = true;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            program_file = argv[++i];
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            memory_file = argv[++i];
        } else {
            fprintf(stderr, "Usage: %s [-s] [-d] [-t] [-p <program file>] [-m <memory file>]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    PDP7 pdp7_minicomputer;

    run_pdp7(&pdp7_minicomputer, use_teleprinter, program_file, memory_file, debug, single_instruction);

    return EXIT_SUCCESS;
}