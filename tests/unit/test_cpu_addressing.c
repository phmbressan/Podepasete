#include "test_cpu_addressing.h"

void test_direct_address(void) {
    PDP7 cpu = create_cpu_with_memory();

    uint32_t addr1 = get_effective_address(&cpu, 00, false);
    uint32_t addr2 = get_effective_address(&cpu, 01, false);
    uint32_t addr3 = get_effective_address(&cpu, 02, false);

    assert_(addr1 == 0000000, "Error on direct addressing.");
    assert_(addr2 == 0000001, "Error on direct addressing.");
    assert_(addr3 == 0000002, "Error on direct addressing.");
}

void test_indirect_address(void) {
    PDP7 cpu = create_cpu_with_memory();

    uint32_t addr1 = get_effective_address(&cpu, 00, true);
    uint32_t addr2 = get_effective_address(&cpu, 01, true);
    uint32_t addr3 = get_effective_address(&cpu, 02, true);

    assert_(addr1 == 0000001, "Error on indirect addressing.");
    assert_(addr2 == 0000002, "Error on indirect addressing.");
    assert_(addr3 == 0000003, "Error on indirect addressing.");
}