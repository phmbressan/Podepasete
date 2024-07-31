#include "sample_cpus.h"

PDP7 create_empty_cpu(void) {
    PDP7 empty_cpu;

    initialize_pdp7(&empty_cpu);

    return empty_cpu;
}

PDP7 create_cpu_with_memory(void) {
    PDP7 cpu_with_memory;

    initialize_pdp7(&cpu_with_memory);
    load_memory_from_file(&cpu_with_memory, "tests/fixtures/data/halt_program.dat", INSTRUCTION_START);
    load_memory_from_file(&cpu_with_memory, "tests/fixtures/data/sample_memory.dat", 0);

    return cpu_with_memory;
}

