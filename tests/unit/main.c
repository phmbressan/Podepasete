#include "test_cpu_setup.h"
#include "test_cpu_addressing.h"
#include "test_cpu_decode.h"

int main() {
    printf("Running tests...\n");

    test_empty_pdp7();
    test_mem_load_pdp7();

    test_direct_address();
    test_indirect_address();

    test_instruction_parsing();

    printf("All tests passed!\n");

    return 0;
}