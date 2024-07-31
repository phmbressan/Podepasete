#include "pdp7_cpu.h"
#include "teleprinter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// PDP-7 Opcodes in octal
#define OP_CAL  000 // Call subroutine and load accumulator
#define OP_DAC  004 // Deposit accumulator into memory
#define OP_JMS  010 // Jump to subroutine
#define OP_DZM  014 // Deposit zero into memory
#define OP_LAC  020 // Load accumulator from memory
#define OP_XOR  024 // Exclusive OR
#define OP_ADD  030 // Add to accumulator
#define OP_TAD  034 // Two's complement add to accumulator
#define OP_XCT  040 // Execute instruction
#define OP_ISZ  044 // Increment and skip if zero
#define OP_AND  050 // Logical AND with accumulator
#define OP_SAD  054 // Skip on accumulator different
#define OP_JMP  060 // Jump
#define OP_IOT  070 // Input/Output Transfer
#define OP_OPR  074 // Operate
#define OP_EAE  077 // Extended arithmetic element (stubbed)

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

void initialize_pdp7(PDP7 *cpu) {
    cpu->accumulator = 0;
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        cpu->memory[i] = 0;
    }
    cpu->memory_address = 0;
    cpu->memory_buffer = 0;
    cpu->pc = INSTRUCTION_START;
    cpu->ir = 0;
    cpu->link = 0;
    cpu->cycles = 0;
    cpu->running = true;
}

void load_memory_from_file(PDP7 *cpu, const char *filename, uint32_t start_address) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file %s\n", filename);
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
uint32_t get_effective_address(PDP7* cpu, uint32_t address, bool indirect) {
    if (indirect) {
        cpu->cycles++;
        // Single level of indirection
        return cpu->memory[address & 017777];
    } else {
        return address;
    }
}

void perform_cycle(PDP7* cpu) {
    uint32_t instruction = cpu->memory[cpu->pc++];

    decode_instruction(cpu, instruction);

    execute_instruction(cpu, instruction);
}

void decode_instruction(PDP7* cpu, uint32_t instruction) {
    uint32_t opcode = (instruction >> 12) & 074; // First 4 bits
    uint32_t address = instruction & 017777; // Last 13 bits
    bool indirect = instruction & 020000; // Indirect bit (bit 5)

    cpu->ir = opcode;
    cpu->memory_address = get_effective_address(cpu, address, indirect);
    cpu->memory_buffer = cpu->memory[cpu->memory_address];
}

void execute_instruction(PDP7* cpu, uint32_t instruction) {
    switch (cpu->ir) {
        case OP_CAL:
            // Call subroutine and load accumulator
            cpu->memory[cpu->memory_address] = cpu->pc;
            cpu->pc = cpu->memory_address + 1;
            cpu->accumulator = cpu->memory[cpu->memory_address];
            cpu->cycles += 2;
            break;
        case OP_DAC:
            // Deposit accumulator into memory
            cpu->memory[cpu->memory_address] = cpu->accumulator;
            cpu->cycles += 2;
            break;
        case OP_JMS:
            // Jump to subroutine
            cpu->memory[cpu->memory_address] = cpu->pc + (cpu->link << 17);
            cpu->pc = cpu->memory_address + 1;
            cpu->cycles += 2;
            break;
        case OP_DZM:
            // Deposit zero into memory
            cpu->memory[cpu->memory_address] = 0;
            cpu->cycles += 2;
            break;
        case OP_LAC:
            // Load accumulator from memory
            cpu->accumulator = cpu->memory[cpu->memory_address];
            cpu->cycles += 2;
            break;
        case OP_XOR:
            // Exclusive OR
            cpu->accumulator ^= cpu->memory[cpu->memory_address];
            cpu->cycles += 2;
            break;
        case OP_ADD:
            cpu->accumulator += cpu->memory[cpu->memory_address];
            cpu->link = cpu->accumulator >> 18; // Save carry in link
            cpu->accumulator = (cpu->accumulator + cpu->link) & 0777777;
            if (cpu->accumulator == 0777777) {
                cpu->accumulator = 0;
            }
            cpu->cycles += 2;
            break;
        case OP_TAD:
            cpu->accumulator += cpu->memory[cpu->memory_address];
            cpu->link = cpu->accumulator >> 18; // Save carry in link
            cpu->accumulator = (cpu->accumulator + cpu->link) & 0777777;
            cpu->cycles += 2;
            break;
        case OP_XCT:
            // Execute instruction
            uint32_t next_instruction = cpu->memory[cpu->memory_address];
            decode_instruction(cpu, next_instruction);
            execute_instruction(cpu, next_instruction);
            cpu->cycles += 1;
            break;
        case OP_ISZ:
            // Increment and skip if zero
            cpu->memory[cpu->memory_address]++;
            if (cpu->memory[cpu->memory_address] == 0) {
                cpu->pc++;
            }
            cpu->cycles += 2;
            break;
        case OP_AND:
            // Logical AND with accumulator
            cpu->accumulator &= cpu->memory[cpu->memory_address];
            cpu->cycles += 2;
            break;
        case OP_SAD:
            // Skip on accumulator different
            if (cpu->accumulator != cpu->memory[cpu->memory_address]) {
                cpu->pc++;
            }
            cpu->cycles += 2;
            break;
        case OP_JMP:
            // Jump
            cpu->pc = cpu->memory_address;
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
                    cpu->accumulator = (uint32_t)input_char;
                    break;
                case IOT_TLS:
                    // Load teleprinter buffer and select, clear teleprinter flag
                    printf("%c", cpu->accumulator & 0xFF); // Print the character in AC
                    break;
                default: 
                    fprintf(stderr, "Unknown I/O instruction: %o\n", cpu->ir);
                    exit(1);
            }
            break;
        case OP_OPR:       
            switch (instruction) {
                case OP_OPR:
                    // No operation
                    cpu->cycles += 3;
                    break;
                case OPR_CMA:
                    // Complement AC
                    cpu->accumulator = ~cpu->accumulator & 0777777;
                    cpu->cycles += 3;
                    break;
                case OPR_CML:
                    // Complement link
                    cpu->link = !cpu->link;
                    cpu->cycles += 3;
                    break;
                case OPR_OAS:
                    // Inclusive OR AC switches
                    cpu->accumulator |= cpu->memory[cpu->memory_address];
                    cpu->cycles += 3;
                    break;
                case OPR_LAS:
                    // Load AC from switches
                    cpu->accumulator = cpu->memory[cpu->memory_address];
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
            fprintf(stderr, "Unknown opcode: %o\n", cpu->ir);
            exit(1);
    }
}

// Print CPU state (for debugging)
void print_cpu_state(const PDP7 *cpu) {
    printf("PC: %o | AC: %o | MEM_ADDR: %o | IR: %o | Cycles: %lu\n",
           cpu->pc, cpu->accumulator, cpu->memory_address, cpu->ir, cpu->cycles);
}
