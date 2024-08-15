#include "pdp7_cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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

// I/O Instructions
#define IOT_KRB 0700312
#define IOT_TLS 0700406

uint32_t program_start_address;

void load_memory_from_file(PDP7_cpu *cpu, const char *filename, uint32_t start_address);
void perform_cycle(PDP7_cpu *cpu);
void decode_instruction(PDP7_cpu *cpu, uint32_t instruction);
void execute_instruction(PDP7_cpu* cpu, uint32_t instruction);
void print_cpu_state(const PDP7_cpu *cpu);
void print_memory(const PDP7_cpu *cpu, uint32_t start, uint32_t end);
uint32_t get_effective_address(PDP7_cpu *cpu, uint32_t address, bool indirect);

void* run_cpu(void* arg) {
    PDP7_cpu_options* cpu_options = (PDP7_cpu_options*)arg;

    PDP7_cpu* cpu = &cpu_options->cpu;

    if (!cpu_options->headless) {
        print_cpu_state(cpu);
        printf("Commands: [n]ext, [c]ontinue, [m]emory, [q]uit\n");

        char command[10];
        while (1) {
            printf("> ");
            if (fgets(command, sizeof(command), stdin)) {
                command[strcspn(command, "\r\n")] = '\0';
                if (strcmp(command, "n") == 0) {
                    perform_cycle(cpu);
                    print_cpu_state(cpu);
                } else if (strcmp(command, "c") == 0) {
                    while (cpu->running) {
                        perform_cycle(cpu);
                        if (cpu_options->debug) {
                            print_cpu_state(cpu);
                        }
                    }
                    break;
                } else if (strcmp(command, "m") == 0) {
                    uint32_t start, end;
                    printf("Enter start address: ");
                    scanf("%o", &start);
                    printf("Enter end address: ");
                    scanf("%o", &end);
                    while (getchar() != '\n');
                    print_memory(cpu, start, end);
                } else if (strcmp(command, "q") == 0) {
                    cpu->running = 0;
                    break;
                } else {
                    printf("Unknown command. Please enter [n], [c], [m], [l], or [q].\n");
                }
            }
        }
    } else {
        while (cpu->running) {
            perform_cycle(cpu);
            if (cpu_options->debug) {
                print_cpu_state(cpu);
            }
        }
    }

    printf("CPU halted\n");
    printf("Total cycles: %lu\n", cpu->cycles);

    if (cpu_options->headless) {
        printf("Press any key to quit.\n");
        getchar();
    } else {
        printf("Commands: [m]emory, [q]uit\n");
        char command[10];
        while (1) {
            printf("> ");
            if (fgets(command, sizeof(command), stdin)) {
                command[strcspn(command, "\r\n")] = '\0';
                if (strcmp(command, "m") == 0) {
                    uint32_t start, end;
                    printf("Enter start address: ");
                    scanf("%o", &start);
                    printf("Enter end address: ");
                    scanf("%o", &end);
                    while (getchar() != '\n');
                    print_memory(cpu, start, end);
                } else if (strcmp(command, "q") == 0) {
                    cpu->running = 0;
                    break;
                } else {
                    printf("Unknown command. Please enter [m] or [q].\n");
                }
            }
        }
    }

    return NULL;
}

void initialize_cpu(PDP7_cpu *cpu, const char* program_file, const char* memory_file, uint32_t* io_buffer, uint32_t start_address) {
    *io_buffer = 0;
    program_start_address = start_address;
    cpu->accumulator = 0;
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        cpu->memory[i] = 0;
    }
    cpu->memory_address = 0;
    cpu->memory_buffer = 0;
    cpu->pc = start_address;
    cpu->ir = 0;
    cpu->io_buffer = io_buffer;
    cpu->link = 0;
    cpu->cycles = 0;
    cpu->running = true;

    if (program_file) {
        load_memory_from_file(cpu, program_file, start_address);
    }

    if (memory_file) {
        load_memory_from_file(cpu, memory_file, 0);
    }
}

void load_memory_from_file(PDP7_cpu *cpu, const char *filename, uint32_t start_address) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file %s\n", filename);
        exit(1);
    }

    char line[256];
    uint32_t address, word;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';

        char *comment_start = strstr(line, "//");
        if (comment_start) {
            *comment_start = '\0';
        }

        if (strlen(line) == 0) {
            continue;
        }

        if (sscanf(line, "%o %o", &address, &word) == 2) {
            if (address >= MEMORY_SIZE) {
                fprintf(stderr, "Invalid address %o in file %s\n", address, filename);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            cpu->memory[start_address + address] = word;
        } else {
            fprintf(stderr, "Invalid format in file %s: %s\n", filename, line);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

uint32_t get_effective_address(PDP7_cpu* cpu, uint32_t address, bool indirect) {
    if (indirect) {
        cpu->cycles++;
        // Pointer indirection handling
        return cpu->memory[address & 017777];
    } else {
        return address;
    }
}

void perform_cycle(PDP7_cpu* cpu) {
    uint32_t instruction = cpu->memory[cpu->pc++];

    decode_instruction(cpu, instruction);

    execute_instruction(cpu, instruction);
}

void decode_instruction(PDP7_cpu* cpu, uint32_t instruction) {
    uint32_t opcode = (instruction >> 12) & 074; // First 4 bits
    uint32_t address = instruction & 017777; // Last 13 bits
    bool indirect = instruction & 020000; // Indirect bit (bit 5)

    cpu->ir = opcode;
    cpu->memory_address = get_effective_address(cpu, address, indirect);
    cpu->memory_buffer = cpu->memory[cpu->memory_address];
}

void execute_instruction(PDP7_cpu* cpu, uint32_t instruction) {
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
            cpu->pc = cpu->memory_address + 1 + program_start_address;
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
            cpu->accumulator = (cpu->accumulator) & 0777777;
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
            cpu->pc = cpu->memory_address + program_start_address;
            cpu->cycles += 1;
            break;
        case OP_IOT:
            // Input/Output Transfer (stubbed)
            switch (instruction) {
                case IOT_KRB:
                    printf(">>> ");
                    char input_char;
                    scanf(" %c", &input_char);
                    cpu->accumulator = (uint32_t)input_char;
                    break;
                case IOT_TLS:
                    while (*cpu->io_buffer != 0);
                    *cpu->io_buffer = cpu->accumulator;
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

void print_memory(const PDP7_cpu *cpu, uint32_t start, uint32_t end) {
    if (start >= MEMORY_SIZE || end >= MEMORY_SIZE || start > end) {
        printf("Invalid memory bounds\n");
        return;
    }

    // Calculate the total number of rows needed
    uint32_t row_start = start - (start % 8);  // Adjust start to the previous multiple of NUM_COLUMNS
    uint32_t row_end = end + (8 - (end % 8)) - 1;  // Adjust end to the next multiple of NUM_COLUMNS - 1

    if (row_end >= MEMORY_SIZE) {
        row_end = MEMORY_SIZE - 1;  // Cap end to the maximum address
    }

    printf("Memory contents from %04" PRIo32 " to %04" PRIo32 ":\n", start, end);

    for (uint32_t addr = row_start; addr <= row_end; addr += 8) {
        printf("%04" PRIo32 ": ", addr);

        for (uint32_t col = 0; col < 8; ++col) {
            uint32_t current_addr = addr + col;
            if (current_addr >= start && current_addr <= end) {
                printf("%0*" PRIo32 " ", 6, cpu->memory[current_addr]);
            } else {
                printf("%*s ", 8, " ");  // Print spaces for out-of-bound addresses
            }
        }

        printf("\n");
    }
}

void print_cpu_state(const PDP7_cpu *cpu) {
    const int WIDTH = 6;

    printf("PC: %*o | AC: %*o | MA: %*o | IR: %*o | Cycles: %*lu\n",
           WIDTH, cpu->pc,
           WIDTH, cpu->accumulator,
           WIDTH, cpu->memory_address,
           WIDTH, cpu->ir,
           WIDTH, cpu->cycles);
}
