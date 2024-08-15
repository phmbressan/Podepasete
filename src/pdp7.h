#pragma once

#include "pdp7_cpu.h"
#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    PDP7_cpu cpu;
    display_340 display;
} PDP7;

void run_pdp7(PDP7 *pdp7, bool use_display, const char *program_file, const char *memory_file, bool debug, bool single_instruction);