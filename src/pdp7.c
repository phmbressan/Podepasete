#include "pdp7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void run_pdp7(PDP7 *pdp7, bool use_display, const char *program_file, const char *memory_file, bool debug, bool single_instruction) {
    uint32_t io_buffer = 0;

    pthread_t threads[2]; 

    if (use_display) {
        initialize_display(&pdp7->display, &io_buffer);
        pthread_create(&threads[0], NULL, run_display, &pdp7->display);
    }

    initialize_cpu(&pdp7->cpu, program_file, memory_file, &io_buffer, debug, single_instruction);
    pthread_create(&threads[1], NULL, run_cpu, &pdp7->cpu);

    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }
}