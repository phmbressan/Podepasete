#include "test_cpu_setup.h"

void test_empty_pdp7(void) {
    PDP7_cpu sample_cpu = create_empty_cpu();

    // Test that the CPU is initialized correctly
    assert_(sample_cpu.accumulator == 0, "Accumulator is not initialized to 0");
    assert_(sample_cpu.pc == INSTRUCTION_START, "Program counter is not initialized to instruction start.");
    assert_(sample_cpu.ir == 0, "Instruction register is not initialized to 0.");
    assert_(sample_cpu.link == 0, "Link register is not initialized to 0.");
    assert_(sample_cpu.cycles == 0, "The initial cycle count is not 0.");
    assert_(sample_cpu.running == true, "The CPU is not running.");

    // Test that the memory is initialized correctly
    for (int i = 0; i < MEMORY_SIZE; i++) {
        assert_(sample_cpu.memory[i] == 0, "Memory is not initialized to 0.");
    }
}

void test_mem_load_pdp7(void) {
    PDP7_cpu sample_cpu = create_empty_cpu();

    load_memory_from_file(&sample_cpu, "tests/fixtures/data/halt_program.dat", INSTRUCTION_START);
    load_memory_from_file(&sample_cpu, "tests/fixtures/data/sample_memory.dat", 0);

    assert_(sample_cpu.memory[INSTRUCTION_START] == 0300000, "Instruction memory is not loaded correctly.");
    assert_(sample_cpu.memory[INSTRUCTION_START + 1] == 0700312, "Instruction memory is not loaded correctly.");
    assert_(sample_cpu.memory[0] == 0000001, "Data memory is not loaded correctly.");
}

