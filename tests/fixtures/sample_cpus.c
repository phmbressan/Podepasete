#include "sample_cpus.h"

PDP7_cpu create_empty_cpu(void) {
    PDP7_cpu empty_cpu;

    uint32_t io_buffer = 0;

    initialize_cpu(&empty_cpu, NULL, NULL, &io_buffer, 02000);

    return empty_cpu;
}

PDP7_cpu create_cpu_with_memory(void) {
    PDP7_cpu cpu_with_memory;

    uint32_t io_buffer = 0;

    initialize_cpu(&cpu_with_memory, "tests/fixtures/data/halt_program.dat", "tests/fixtures/data/sample_memory.dat", &io_buffer, 02000);

    return cpu_with_memory;
}

