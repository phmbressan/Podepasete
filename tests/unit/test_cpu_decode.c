#include "test_cpu_decode.h"

void test_instruction_parsing() {
    PDP7 cpu = create_cpu_with_memory();

    decode_instruction(&cpu, 0300000); // ADD
    assert_(cpu.ir == 030, "Failed to parse ADD opcode");
    assert_(cpu.memory_address == 00000, "Failed to parse ADD address");
}