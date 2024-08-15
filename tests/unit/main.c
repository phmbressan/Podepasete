#include "test_cpu_setup.h"
#include "test_cpu_addressing.h"
#include "test_cpu_decode.h"
#include "test_cpu_execute.h"

int main(void);

int main() {
    printf("Running tests...\n");

    printf("Testing CPU setup...\n");
    test_empty_pdp7();
    test_mem_load_pdp7();

    printf("Testing CPU addressing...\n");
    test_direct_address();
    test_indirect_address();

    printf("Testing CPU decoding...\n");
    test_instruction_parsing();

    printf("Testing CPU execution...\n");
    test_execute_add();
    test_execute_tad();
    test_execute_lac();

    printf("All tests passed!\n");

    return 0;
}