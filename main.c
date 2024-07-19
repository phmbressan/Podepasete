#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define MEMORY_SIZE 8192 // PDP-7 has 13-bit addressing, giving 8K words of memory
#define INSTRUCTION_START 02000 // Start of instruction memory (octal 2000)

// PDP-7 Opcodes in octal
#define OP_CAL  0000000 // Call subroutine and load accumulator
#define OP_DAC  0040000 // Deposit accumulator into memory
#define OP_JMS  0100000 // Jump to subroutine
#define OP_DZM  0140000 // Deposit zero into memory
#define OP_LAC  0200000 // Load accumulator from memory
#define OP_XOR  0240000 // Exclusive OR
#define OP_ADD  0300000 // Add to accumulator
#define OP_TAD  0340000 // Two's complement add to accumulator
#define OP_XCT  0400000 // Execute instruction
#define OP_ISZ  0440000 // Increment and skip if zero
#define OP_AND  0500000 // Logical AND with accumulator
#define OP_SAD  0540000 // Skip on accumulator different
#define OP_JMP  0600000 // Jump
#define OP_EAE  0740000 // Extended arithmetic element (stubbed)
#define OP_IOT  0700000 // Input/Output Transfer (stubbed)
#define OP_OPR  0740000 // Operate

// Operate instructions
#define OPR_CMA  0740001 // Complement AC
#define OPR_CML  0740002 // Complement link
#define OPR_OAS  0740004 // Inclusive OR AC switches
#define OPR_LAS  0750004 // Load AC from switches
#define OPR_RAL  0740010 // Rotate AC + link left one place
#define OPR_RCL  0744010 // Clear link, then rotate left one place
#define OPR_RTL  0742010 // Rotate AC left twice
#define OPR_RAR  0740020 // Rotate AC + link right one place
#define OPR_RCR  0744020 // Clear link, then rotate right one place
#define OPR_RTR  0742020 // Rotate AC right twice
#define OPR_HLT  0740040 // Halt
#define OPR_SZA  0740200 // Skip on zero AC
#define OPR_SNA  0741200 // Skip on non-zero AC
#define OPR_SPA  0741100 // Skip on positive AC
#define OPR_SMA  0740100 // Skip on negative AC
#define OPR_SZL  0741400 // Skip on zero link
#define OPR_SNL  0740400 // Skip on non-zero link
#define OPR_SKP  0471000 // Skip unconditionally
#define OPR_CLL  0744000 // Clear link
#define OPR_STL  0744002 // Set the link
#define OPR_CLA  0750000 // Clear AC
#define OPR_CLC  0750001 // Clear and complement AC
#define OPR_GLK  0750020 // Get link

// EAE Instructions
#define EAE_LRS  0640500
#define EAE_LRSS 0660500
#define EAE_LLS  0640600
#define EAE_LLSS 0660600
#define EAE_ALS  0640700
#define EAE_ALSS 0660700
#define EAE_NORM 0640444
#define EAE_NORMS 0660444
#define EAE_MUL  0653122
#define EAE_MULS 0657122
#define EAE_DIV  0640323
#define EAE_DIVS 0644323
#define EAE_IDIV 0653323
#define EAE_IDIVS 0657323
#define EAE_FRDIV 0650323
#define EAE_FRDIVS 0654323
#define EAE_LACQ 0641002
#define EAE_LACS 0641001
#define EAE_CLQ  0650000
#define EAE_ABS  0644000
#define EAE_GSM  0664000
#define EAE_OSC  0640001
#define EAE_OMQ  0640002
#define EAE_CMQ  0640004

// I/O Instructions
#define IOT_KRB 0700312
#define IOT_TLS 0700406

typedef struct {
    uint32_t accumulator;            // Accumulator (18-bit)
    uint32_t memory[MEMORY_SIZE];    // Memory array (18-bit words)
    uint32_t pc;                     // Program Counter (13-bit)
    uint32_t ir;                     // Instruction Register (18-bit)
    uint8_t link;                    // Link Register (1-bit)
    uint64_t cycles;                 // Cycle counter
    bool running;                    // CPU running state
    bool debug;                      // CPU Debug Mode
} PDP7;

void initialize_pdp7(PDP7 *cpu);
void load_memory_from_file(PDP7 *cpu, const char *filename, uint32_t start_address);
void execute_instruction(PDP7 *cpu);
void decode_instruction(PDP7 *cpu, uint32_t instruction);
void print_cpu_state(const PDP7 *cpu);
uint32_t get_effective_address(PDP7 *cpu, uint32_t address, bool indirect);

int main(int argc, char *argv[]) {
    PDP7 cpu;
    initialize_pdp7(&cpu);

    char *program_file = "program.dat";
    char *memory_file = "memory.dat";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            cpu.debug = true;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            program_file = argv[++i];
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            memory_file = argv[++i];
        } else {
            fprintf(stderr, "Usage: %s [-d] [-p <program file>] [-m <memory file>]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Load instruction memory from file
    load_memory_from_file(&cpu, program_file, INSTRUCTION_START);

    // Load data memory from file
    load_memory_from_file(&cpu, memory_file, 0);

    // Main emulation loop
    while (cpu.running) {
        execute_instruction(&cpu);

        if (cpu.debug) {
            print_cpu_state(&cpu);
        }
    }

    return EXIT_SUCCESS;
}

void initialize_pdp7(PDP7 *cpu) {
    cpu->accumulator = 0;
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        cpu->memory[i] = 0;
    }
    cpu->pc = INSTRUCTION_START;
    cpu->ir = 0;
    cpu->link = 0;
    cpu->cycles = 0;
    cpu->running = true;
    cpu->debug = false;
}

void load_memory_from_file(PDP7 *cpu, const char *filename, uint32_t start_address) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    uint32_t address, word;
    while (fscanf(file, "%o %o", &address, &word) == 2) {
        if (address >= MEMORY_SIZE) {
            fprintf(stderr, "Invalid address %o in file %s\n", address, filename);
            fclose(file);
            exit(1);
        }
        cpu->memory[start_address + address] = word;
    }

    fclose(file);
}

// Fetch the effective address
uint32_t get_effective_address(PDP7 *cpu, uint32_t address, bool indirect) {
    if (indirect) {
        // Single level of indirection
        return cpu->memory[address & 017777];
    } else {
        return address;
    }
}

// Execute a single instruction
void execute_instruction(PDP7 *cpu) {
    // Fetch instruction
    cpu->ir = cpu->memory[cpu->pc];
    // Increment program counter
    cpu->pc++;
    // Decode and execute instruction
    decode_instruction(cpu, cpu->ir);
}

// Decode and execute an instruction
void decode_instruction(PDP7 *cpu, uint32_t instruction) {
    uint32_t opcode = instruction & 0760000; // Extract the opcode (first 4+1 bits)
    uint32_t address = instruction & 017777; // Extract the address field (last 13 bits)
    bool indirect = instruction & 020000; // Extract the indirect bit

    uint32_t effective_address = get_effective_address(cpu, address, indirect);

    switch (opcode) {
        case OP_CAL:
            // Call subroutine and load accumulator
            cpu->memory[effective_address] = cpu->pc;
            cpu->pc = effective_address + 1;
            cpu->accumulator = cpu->memory[effective_address];
            cpu->cycles += 2;
            break;
        case OP_DAC:
            // Deposit accumulator into memory
            cpu->memory[effective_address] = cpu->accumulator;
            cpu->cycles += 2;
            break;
        case OP_JMS:
            // Jump to subroutine
            cpu->memory[effective_address] = cpu->pc;
            cpu->pc = effective_address + 1;
            cpu->cycles += 2;
            break;
        case OP_DZM:
            // Deposit zero into memory
            cpu->memory[effective_address] = 0;
            cpu->cycles += 2;
            break;
        case OP_LAC:
            // Load accumulator from memory
            cpu->accumulator = cpu->memory[effective_address];
            cpu->cycles += 2;
            break;
        case OP_XOR:
            // Exclusive OR
            cpu->accumulator ^= cpu->memory[effective_address];
            cpu->cycles += 2;
            break;
        case OP_ADD:
            // Add to accumulator
            cpu->accumulator += cpu->memory[effective_address];
            if (cpu->accumulator & 0x20000) { // Check for carry out of 18-bit
                cpu->accumulator = (cpu->accumulator & 0x1FFFF) + 1;
                cpu->link ^= 1; // Toggle link bit on carry
            }
            cpu->cycles += 2;
            break;
        case OP_TAD:
            // Two's complement add to accumulator
            cpu->accumulator += cpu->memory[effective_address];
            if (cpu->accumulator & 0x20000) { // Check for carry out of 18-bit
                cpu->accumulator = (cpu->accumulator & 0x1FFFF) + 1;
                cpu->link ^= 1; // Toggle link bit on carry
            }
            cpu->cycles += 2;
            break;
        case OP_XCT:
            // Execute instruction
            decode_instruction(cpu, cpu->memory[effective_address]);
            cpu->cycles += 1;
            break;
        case OP_ISZ:
            // Increment and skip if zero
            cpu->memory[effective_address]++;
            if (cpu->memory[effective_address] == 0) {
                cpu->pc++;
            }
            cpu->cycles += 2;
            break;
        case OP_AND:
            // Logical AND with accumulator
            cpu->accumulator &= cpu->memory[effective_address];
            cpu->cycles += 2;
            break;
        case OP_SAD:
            // Skip on accumulator different
            if (cpu->accumulator != cpu->memory[effective_address]) {
                cpu->pc++;
            }
            cpu->cycles += 2;
            break;
        case OP_JMP:
            // Jump
            cpu->pc = effective_address;
            cpu->cycles += 1;
            break;
        case OP_IOT:
            // Input/Output Transfer (stubbed)
            switch (instruction) {
                case IOT_KRB:
                    // Read the keyboard buffer into the AC, clear keyboard flag
                    printf(">>> ");
                    char input_char;
                    scanf(" %c", &input_char);
                    cpu->accumulator = input_char;
                    break;
                case IOT_TLS:
                    // Load teleprinter buffer and select, clear teleprinter flag
                    printf("%c", cpu->accumulator & 0xFF); // Print the character in AC
                    break;
                }
            break;
        case OP_OPR:
            // Operate instructions
            switch (instruction) {
                case OP_OPR:
                    // No operation
                    cpu->cycles += 3;
                    break;
                case OPR_CMA:
                    // Complement AC
                    cpu->accumulator = ~cpu->accumulator & 0x3FFFF;
                    cpu->cycles += 3;
                    break;
                case OPR_CML:
                    // Complement link
                    cpu->link = !cpu->link;
                    cpu->cycles += 3;
                    break;
                case OPR_OAS:
                    // Inclusive OR AC switches
                    cpu->accumulator |= cpu->memory[effective_address];
                    cpu->cycles += 3;
                    break;
                case OPR_LAS:
                    // Load AC from switches
                    cpu->accumulator = cpu->memory[effective_address];
                    cpu->cycles += 2;
                    break;
                case OPR_RAL:
                    // Rotate AC + link left one place
                    {
                        uint32_t combined = (cpu->accumulator << 1) | cpu->link;
                        cpu->link = (combined >> 18) & 1;
                        cpu->accumulator = combined & 0x3FFFF;
                        cpu->cycles += 3;
                    }
                    break;
                case OPR_RCL:
                    // Clear link, then rotate left one place
                    cpu->link = 0;
                    cpu->accumulator = (cpu->accumulator << 1) & 0x3FFFF;
                    cpu->cycles += 2;
                    break;
                case OPR_RTL:
                    // Rotate AC left twice
                    {
                        uint32_t combined = (cpu->accumulator << 2) | (cpu->link << 1);
                        cpu->link = (combined >> 18) & 1;
                        cpu->accumulator = combined & 0x3FFFF;
                        cpu->cycles += 2;
                    }
                    break;
                case OPR_RAR:
                    // Rotate AC + link right one place
                    {
                        uint32_t combined = (cpu->link << 18) | cpu->accumulator;
                        cpu->link = combined & 1;
                        cpu->accumulator = (combined >> 1) & 0x3FFFF;
                        cpu->cycles += 2;
                    }
                    break;
                case OPR_RCR:
                    // Clear link, then rotate right one place
                    cpu->link = 0;
                    cpu->accumulator = (cpu->accumulator >> 1) & 0x3FFFF;
                    cpu->cycles += 2;
                    break;
                case OPR_RTR:
                    // Rotate AC right twice
                    {
                        uint32_t combined = (cpu->link << 19) | (cpu->accumulator << 1);
                        cpu->link = (combined >> 18) & 1;
                        cpu->accumulator = (combined >> 2) & 0x3FFFF;
                        cpu->cycles += 2;
                    }
                    break;
                case OPR_HLT:
                    // Halt
                    cpu->running = false;
                    cpu->cycles += 4;
                    break;
                case OPR_SZA:
                    // Skip on zero AC
                    if (cpu->accumulator == 0) {
                        cpu->pc++;
                    }
                    cpu->cycles += 1;
                    break;
                case OPR_SNA:
                    // Skip on non-zero AC
                    if (cpu->accumulator != 0) {
                        cpu->pc++;
                    }
                    cpu->cycles += 1;
                    break;
                case OPR_SPA:
                    // Skip on positive AC
                    if ((cpu->accumulator & 0x20000) == 0) {
                        cpu->pc++;
                    }
                    cpu->cycles += 1;
                    break;
                case OPR_SMA:
                    // Skip on negative AC
                    if (cpu->accumulator & 0x20000) {
                        cpu->pc++;
                    }
                    cpu->cycles += 1;
                    break;
                case OPR_SZL:
                    // Skip on zero link
                    if (cpu->link == 0) {
                        cpu->pc++;
                    }
                    cpu->cycles += 1;
                    break;
                case OPR_SNL:
                    // Skip on non-zero link
                    if (cpu->link != 0) {
                        cpu->pc++;
                    }
                    cpu->cycles += 1;
                    break;
                case OPR_SKP:
                    // Skip unconditionally
                    cpu->pc++;
                    cpu->cycles += 1;
                    break;
                case OPR_CLL:
                    // Clear link
                    cpu->link = 0;
                    cpu->cycles += 2;
                    break;
                case OPR_STL:
                    // Set the link
                    cpu->link = 1;
                    cpu->cycles += 2;
                    break;
                case OPR_CLA:
                    // Clear AC
                    cpu->accumulator = 0;
                    cpu->cycles += 2;
                    break;
                case OPR_CLC:
                    // Clear and complement AC
                    cpu->accumulator = 0x3FFFF;
                    cpu->cycles += 2;
                    break;
                case OPR_GLK:
                    // Get link
                    cpu->accumulator = (cpu->accumulator & 0x1FFFF) | (cpu->link << 17);
                    cpu->cycles += 2;
                    break;
                default:
                    fprintf(stderr, "Unknown operate instruction: %o\n", instruction);
                    exit(1);
            }
            break;
        default:
            fprintf(stderr, "Unknown opcode: %o\n", opcode);
            exit(1);
    }
}

// Print CPU state (for debugging)
void print_cpu_state(const PDP7 *cpu) {
    printf("PC: %o | AC: %o | LINK: %o | IR: %o | Cycles: %lu\n",
           cpu->pc, cpu->accumulator, cpu->link, cpu->ir, cpu->cycles);
}
