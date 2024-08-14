#include "test_cpu_execute.h"

void test_execute_add(void) {
    PDP7_cpu cpu = create_empty_cpu();

    cpu.ir = 030;
    cpu.accumulator = 010;
    cpu.memory[8] = 020;
    cpu.memory_address = 8;

    execute_instruction(&cpu, 0300010); // ADD

    assert_(cpu.accumulator == 030, "Failed to execute ADD instruction");
}

void test_execute_tad(void) {
    PDP7_cpu cpu = create_empty_cpu();

    cpu.ir = 034;
    cpu.accumulator = 010;
    cpu.memory[8] = 020;
    cpu.memory_address = 8;

    execute_instruction(&cpu, 0340010); // TAD

    assert_(cpu.accumulator == 030, "Failed to execute TAD instruction");
}

void test_execute_lac(void) {
    PDP7_cpu cpu = create_empty_cpu();

    cpu.ir = 020;
    cpu.accumulator = 010;
    cpu.memory[8] = 020;
    cpu.memory_address = 8;

    execute_instruction(&cpu, 0200010); // LAC

    assert_(cpu.accumulator == 020, "Failed to execute LAC instruction");
}