#include "pdp7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

uint32_t io_buffer = 0;

void run_pdp7(PDP7 *pdp7, const char *program_file, const char *memory_file, uint32_t start_address, bool use_display, bool debug, bool headless) {
    printf("PDP-7 Minicomputer Emulator\n");
    printf("---------------------------\n\n");

    if (!headless) {
        printf("Running in interactive mode\n");
        printf("Commands: [r]un, [d]ebug, [s]creen, [l]oad, [q]uit\n");

        char command[10];

        while (1) {
            printf("> ");
            if (fgets(command, sizeof(command), stdin)) {
                command[strcspn(command, "\r\n")] = '\0';
                if (strcmp(command, "r") == 0) {
                    if (program_file == NULL) {
                        printf("No program loaded. Please load a program first.\n");
                        continue;
                    }
                    break;
                } else if (strcmp(command, "l") == 0) {
                    char program[256], mem[256];
                    printf("Enter start relocable program start address. (Default 2000): ");
                    scanf("%o", &start_address);
                    printf("Enter program file path. (Default 'data/program.dat'): ");
                    fgets(program, sizeof(program), stdin);
                    if (program[0] == '\n') {
                        strcpy(program, "data/program.dat");
                    }
                    program[strcspn(program, "\r\n")] = '\0';

                    printf("Enter memory file path. (Default 'data/memory.dat'): ");
                    fgets(mem, sizeof(mem), stdin);
                    if (mem[0] == '\n') {
                        strcpy(mem, "data/memory.dat");
                    }
                    mem[strcspn(mem, "\r\n")] = '\0';
                } else if (strcmp(command, "d") == 0) {
                    debug = !debug;
                    printf("Debug mode %s\n", debug ? "enabled" : "disabled");
                } else if (strcmp(command, "s") == 0) {
                    use_display = !use_display;
                    printf("Display %s\n", use_display ? "enabled" : "disabled");
                } else if (strcmp(command, "q") == 0) {
                    exit(0);
                } else {
                    printf("Unknown command. Please enter [r], [d], [s], [l], or [q].\n");
                }
            }
        }  
    } else {
        printf("Running in headless mode\n");
    }

    pthread_t threads[2]; 

    if (use_display) {
        initialize_display(&pdp7->display, &io_buffer);
        pthread_create(&threads[0], NULL, run_display, &pdp7->display);
    }

    initialize_cpu(&pdp7->cpu, program_file, memory_file, &io_buffer, start_address);

    PDP7_cpu_options cpu_options = { .cpu = pdp7->cpu, .debug = debug, .headless = headless };

    pthread_create(&threads[1], NULL, run_cpu, &cpu_options);
    pthread_join(threads[1], NULL);

    if (use_display) {
        pdp7->display.running = false;
        pthread_join(threads[0], NULL);
    }
}

