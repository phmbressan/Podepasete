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
    uint32_t* io_buffer;             // I/O Buffer addr
    uint8_t ir;                      // Instruction Register (4-bit)
    bool link;                       // Link Register (1-bit)
    uint64_t cycles;                 // Cycle counter
    bool running;                    // CPU running state
    bool debug;                      // Debug mode
    bool single_instruction;         // Single instruction mode
} PDP7_cpu;

void* run_cpu(void* arg);
void initialize_cpu(PDP7_cpu *cpu, const char* program_file, const char* memory_file, uint32_t* io_buffer, bool debug, bool single_instruction);