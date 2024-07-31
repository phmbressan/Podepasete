#pragma once

#include <stdint.h>
#include <stdbool.h>

// Constants
#define MEMORY_SIZE 8192 // PDP-7 has 13-bit addressing, giving 8K words of memory
#define INSTRUCTION_START 02000 // Start of instruction memory (octal 2000)

typedef struct {
    uint32_t accumulator;            // Accumulator (18-bit)
    uint32_t memory_address;         // Memory Address (13-bit)
    uint32_t memory_buffer;          // Memory Buffer (18-bit)
    uint32_t memory[MEMORY_SIZE];    // Memory array (18-bit words)
    uint32_t pc;                     // Program Counter (13-bit)
    uint8_t ir;                      // Instruction Register (4-bit)
    bool link;                       // Link Register (1-bit)
    uint64_t cycles;                 // Cycle counter
    bool running;                    // CPU running state
} PDP7;

void initialize_pdp7(PDP7 *cpu);
void load_memory_from_file(PDP7 *cpu, const char *filename, uint32_t start_address);
void perform_cycle(PDP7 *cpu);
void decode_instruction(PDP7 *cpu, uint32_t instruction);
void execute_instruction(PDP7* cpu, uint32_t instruction);
void print_cpu_state(const PDP7 *cpu);
uint32_t get_effective_address(PDP7 *cpu, uint32_t address, bool indirect);