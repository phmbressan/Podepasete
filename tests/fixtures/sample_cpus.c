#include "sample_cpus.h"

PDP7_cpu create_empty_cpu(void) {
    PDP7_cpu empty_cpu;

    initialize_cpu(&empty_cpu, NULL, NULL, NULL, false, false);

    return empty_cpu;
}

PDP7_cpu create_cpu_with_memory(void) {
    PDP7_cpu cpu_with_memory;

    initialize_cpu(&cpu_with_memory, "tests/fixtures/data/halt_program.dat", "tests/fixtures/data/sample_memory.dat", NULL, false, false);

    return cpu_with_memory;
}

