#include "sample_cpus.h"

PDP7_cpu create_empty_cpu(void) {
    PDP7_cpu empty_cpu;

    initialize_cpu(&empty_cpu);

    return empty_cpu;
}

PDP7_cpu create_cpu_with_memory(void) {
    PDP7_cpu cpu_with_memory;

    initialize_cpu(&cpu_with_memory);
    load_memory_from_file(&cpu_with_memory, "tests/fixtures/data/halt_program.dat", INSTRUCTION_START);
    load_memory_from_file(&cpu_with_memory, "tests/fixtures/data/sample_memory.dat", 0);

    return cpu_with_memory;
}

