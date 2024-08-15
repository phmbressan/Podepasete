#include "pdp7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    bool headless = false;
    bool debug = false;
    bool use_display = false;
    uint32_t start_address = INSTRUCTION_START;

    char *program_file = NULL;
    char *memory_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "-h") == 0) {
            headless = true;
        } else if (strcmp(argv[i], "-t") == 0) {
            use_display = true;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            program_file = argv[++i];
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            memory_file = argv[++i];
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            start_address = strtol(argv[++i], NULL, 8);
        } else {
            fprintf(stderr, "Usage: %s [-d] [-t] [-h] [-p <program file>] [-m <memory file>] [-a <start address>]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    PDP7 pdp7_minicomputer;

    run_pdp7(&pdp7_minicomputer, program_file, memory_file, start_address, use_display, debug, headless);

    return EXIT_SUCCESS;
}