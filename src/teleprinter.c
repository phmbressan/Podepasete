#include "teleprinter.h"
#include <stdio.h>
#include <unistd.h>

const int FONT_SIZE = 2;  
const int FONT_WIDTH = 5;
const int FONT_HEIGHT = 7;
const int X_SEP = 1;
const int Y_SEP = 1; 
volatile uint32_t teleprinter_buffer = 0;

const uint8_t FONT_MAP[64][7] = {
    [0]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ' '
	[1]  = {0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00}, // '!'
	[2]  = {0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00}, // '"'
	[3]  = {0x14, 0x7f, 0x14, 0x7f, 0x14, 0x00, 0x00}, // '#'
	[4]  = {0x24, 0x2a, 0x7f, 0x2a, 0x12, 0x00, 0x00}, // '$'
	[5]  = {0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00}, // '%'
	[6]  = {0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x00}, // '&'
	[7]  = {0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00}, // '''
	[8]  = {0x00, 0x1c, 0x22, 0x41, 0x00, 0x00, 0x00}, // '('
	[9]  = {0x00, 0x41, 0x22, 0x1c, 0x00, 0x00, 0x00}, // ')'
	[10] = {0x14, 0x08, 0x3e, 0x08, 0x14, 0x00, 0x00}, // '*'
	[11] = {0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00}, // '+'
	[12] = {0x00, 0x50, 0x30, 0x00, 0x00, 0x00, 0x00}, // ','
	[13] = {0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00}, // '-'
	[14] = {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00}, // '.'
	[15] = {0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00}, // '/'
    [16] = {0x3e, 0x51, 0x49, 0x45, 0x3e, 0x00, 0x00}, // '0'
    [17] = {0x00, 0x42, 0x7f, 0x40, 0x00, 0x00, 0x00}, // '1'
    [18] = {0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00}, // '2'
    [19] = {0x21, 0x41, 0x45, 0x4b, 0x31, 0x00, 0x00}, // '3'
    [20] = {0x18, 0x14, 0x12, 0x7f, 0x10, 0x00, 0x00}, // '4'
    [21] = {0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00}, // '5'
    [22] = {0x3c, 0x4a, 0x49, 0x49, 0x30, 0x00, 0x00}, // '6'
    [23] = {0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00}, // '7'
    [24] = {0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00}, // '8'
    [25] = {0x06, 0x49, 0x49, 0x29, 0x1e, 0x00, 0x00}, // '9'
    [26] = {0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00}, // ':'
    [27] = {0x00, 0x56, 0x36, 0x00, 0x00, 0x00, 0x00}, // ';'
    [28] = {0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00}, // '<'
    [29] = {0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00}, // '='
    [30] = {0x00, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00}, // '>'
    [31] = {0x02, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00}, // '?'
    [32] = {0x32, 0x49, 0x79, 0x41, 0x3e, 0x00, 0x00}, // '@'
    [33] = {0x7e, 0x11, 0x11, 0x11, 0x7e, 0x00, 0x00}, // 'A'
    [34] = {0x7f, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00}, // 'B'
    [35] = {0x3e, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00}, // 'C'
    [36] = {0x7f, 0x41, 0x41, 0x22, 0x1c, 0x00, 0x00}, // 'D'
    [37] = {0x7f, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00}, // 'E'
    [38] = {0x7f, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00}, // 'F'
    [39] = {0x3e, 0x41, 0x49, 0x49, 0x7a, 0x00, 0x00}, // 'G'
    [40] = {0x7f, 0x08, 0x08, 0x08, 0x7f, 0x00, 0x00}, // 'H'
    [41] = {0x00, 0x41, 0x7f, 0x41, 0x00, 0x00, 0x00}, // 'I'
    [42] = {0x20, 0x40, 0x41, 0x3f, 0x01, 0x00, 0x00}, // 'J'
    [43] = {0x7f, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00}, // 'K'
    [44] = {0x7f, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00}, // 'L'
    [45] = {0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00, 0x00}, // 'M'
    [46] = {0x7f, 0x04, 0x08, 0x10, 0x7f, 0x00, 0x00}, // 'N'
    [47] = {0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00}, // 'O'
    [48] = {0x7f, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00}, // 'P'
    [49] = {0x3e, 0x41, 0x51, 0x21, 0x5e, 0x00, 0x00}, // 'Q'
    [50] = {0x7f, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00}, // 'R'
    [51] = {0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00}, // 'S'
    [52] = {0x01, 0x01, 0x7f, 0x01, 0x01, 0x00, 0x00}, // 'T'
    [53] = {0x3f, 0x40, 0x40, 0x40, 0x3f, 0x00, 0x00}, // 'U'
    [54] = {0x1f, 0x20, 0x40, 0x20, 0x1f, 0x00, 0x00}, // 'V'
    [55] = {0x3f, 0x40, 0x38, 0x40, 0x3f, 0x00, 0x00}, // 'W'
    [56] = {0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00}, // 'X'
    [57] = {0x07, 0x08, 0x70, 0x08, 0x07, 0x00, 0x00}, // 'Y'
    [58] = {0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00}, // 'Z'
    [59] = {0x00, 0x7f, 0x41, 0x41, 0x00, 0x00, 0x00}, // '['
	[60] = {0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00}, // '/'
	[61] = {0x00, 0x41, 0x41, 0x7f, 0x00, 0x00, 0x00}, // ']'
	[62] = {0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00}, // '^'
	[63] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00}  // '_'
};

void set_pixel(int x, int y, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

void draw_char(int x, int y, uint8_t char_code, SDL_Renderer *renderer) {
    for (int row = 0; row < FONT_WIDTH; row++) {
        for (int col = 0; col < FONT_HEIGHT; col++) {
            if (FONT_MAP[char_code][row] & (1 << col)) {
                for (int i = 0; i < FONT_SIZE; i++) {
                    for (int j = 0; j < FONT_SIZE; j++) {
                        set_pixel(x + row * FONT_SIZE + i, y + col*FONT_SIZE + j, renderer);
                    }
                }
            }
        }
    }
}

void handle_instruction(uint32_t instruction, SDL_Renderer *renderer, int *mode) {
    printf("Processing instruction: %06o with mode %d\n", instruction, *mode); // Debug output for instruction

    switch (*mode) {
        case 0: // Parameter mode
            *mode = (instruction >> 13) & 0x7;
            printf("Parameter mode: Next mode set to %d\n", *mode); // Debug output for mode change
            break;
        case 1: // Point mode
        {
            bool axis = (instruction >> 16) & 0x1;
            int next_mode = (instruction >> 13) & 0x7;
            bool lit = (instruction >> 10) & 0x1;
            int coord = instruction & 0x3FF;

            SDL_SetRenderDrawColor(renderer, 0, lit ? 255 : 0, 0, 255);
            if (axis == 0) {
                SDL_RenderDrawPoint(renderer, coord, SCREEN_HEIGHT / 2);
                printf("Point mode: Drawing point at (%d, %d), lit = %d\n", coord, SCREEN_HEIGHT / 2, lit); // Debug output for drawing
            } else {
                SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT - coord);
                printf("Point mode: Drawing point at (%d, %d), lit = %d\n", SCREEN_WIDTH / 2, SCREEN_HEIGHT - coord, lit); // Debug output for drawing
            }
            *mode = next_mode;
        }
        break;
        case 3: // Char mode
            uint32_t curr_inst = instruction;
            uint8_t curr_char;
            bool escape = false;
            static int x_delta = 0, y_delta = 0;

            for (int i = 1; i <= 3; i++) {
                curr_char = (curr_inst & 0x3F000) >> 12;
                curr_inst = curr_inst << 6;
                escape = (curr_char == 58);

                if (!escape) {
                    draw_char(x_delta, y_delta, curr_char, renderer);
                    x_delta += (FONT_WIDTH + X_SEP) * FONT_SIZE;
                    if (x_delta >= SCREEN_WIDTH - FONT_SIZE * FONT_WIDTH) {
                        x_delta = 0;
                        y_delta += (FONT_HEIGHT + Y_SEP) * FONT_SIZE;
                    }
                } else {
                    *mode = 0;
                    break;
                }
            }

            printf("Char mode: Instruction = %06o, Next mode = %d\n", instruction, *mode); // Debug output for char mode
            break;
        case 4: // Vector mode
        {
            bool stay_in_mode = ((instruction >> 17) & 0x1) == 0;
            bool lit = (instruction >> 16) & 0x1;
            bool neg_y = (instruction >> 15) & 0x1;
            bool neg_x = (instruction >> 7) & 0x1;
            int dy = ((instruction >> 8) & 0xFF);
            int dx = (instruction & 0xFF);

            dy = neg_y ? -dy : dy;
            dx = neg_x ? -dx : dx;

            SDL_SetRenderDrawColor(renderer, 0, lit ? 255 : 0, 0, 255);
            SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 + dx, SCREEN_HEIGHT / 2 - dy);
            printf("Vector mode: Drawing line to (%d + %d, %d - %d), lit = %d\n", SCREEN_WIDTH / 2, dx, SCREEN_HEIGHT / 2, dy, lit); // Debug output for drawing
            if (!stay_in_mode) {
                *mode = 0;
            }
        }
        break;
        default:
            *mode = 0;
            printf("Default case: Resetting mode to 0\n"); // Debug output for default case
            break;
    }
}


void print_teleprinter(SDL_Renderer* renderer, int* mode){
    handle_instruction(0b000110000000000000, renderer, &mode);
    for(int i=0; i<=2; i++){
        uint8_t current_char = teleprinter_buffer % 8;
        int_to_char(current_char);
        teleprinter_buffer /= 8;
    }
    teleprinter_buffer = 0;
}

uint8_t int_to_char(uint8_t single){
    uint8_t output;

    switch (single) {
        case 0:
            output = 16;
            break;
        case 1:
            output = 17;
            break;
        case 2:
            output = 18;
            break;
        case 3:
            output = 19;
            break;
        case 4:
            output = 20;
            break;
        case 5:
            output = 21;
            break;
        case 6:
            output = 22;
            break;
        case 7:
            output = 23;
            break;
        case 8:
            output = 24;
            break;
        case 9:
            output = 25;
            break;
        default: 
            printf("char not recognized!");
    }

    return output;
}

 int start_teleprinter() {
     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
         printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
         return 1;
     }
     SDL_Window *window = SDL_CreateWindow("340 Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
     if (window == NULL) {
         printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
         SDL_Quit();
         return 1;
     }
     SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
     if (renderer == NULL) {
         printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
         SDL_DestroyWindow(window);
         SDL_Quit();
         return 1;
     }
     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
     SDL_RenderClear(renderer);
     bool quit = false;
     SDL_Event e;
     int mode = 0;
    //  uint32_t instructions[] = {
    //      // Parameter mode to Point mode
    //      0b000010000000000000, // Mode change to Point mode
    //      0b000010011000000000, // Point mode: lit point at x=512
    //      0b011000011000000000, // Point mode: lit point at y=512
    //      // Point mode to Vector mode
    //      0b010100000001000000, // Vector mode: draw a line dx=32, dy=32, lit
    //      0b010000000001000000, // Vector mode: draw a line dx=0, dy=64, lit
    //      0b111100000000000000, // Vector mode: draw a line dx=-32, dy=0, lit
    //      // Return to parameter mode
    //      0b000110000000000000, // Mode change to Char mode
    //      0b000000000001000010,
    //      0b000011000100000101,
    //      0b000110000111001000,
    //      0b000011000100000101,
    //      0b100001100010100011,
    //      0b100001100010100011,
    //      0b100001100010100011,
    //      0b100001100010100011,
    //      0b100001100010100011,
    //      0b100000001111111100
    //  };
    // size_t instruction_count = sizeof(instructions) / sizeof(instructions[0]);
    // size_t instruction_index = 0;
    //  while (!quit) {
    //      while (SDL_PollEvent(&e) != 0) {
    //          if (e.type == SDL_QUIT) {
    //              quit = true;
    //          }
    //      }
    //      if (instruction_index < instruction_count) {
    //          uint32_t instruction = instructions[instruction_index++];
    //          handle_instruction(instruction, renderer, &mode);
    //          SDL_RenderPresent(renderer);
    //          SDL_Delay(500); // Delay to visually distinguish each instruction
    //      }
    //  }
    while (1){
        if (teleprinter_buffer == 0) {continue;}
        print_teleprinter(renderer, &mode);
        handle_instruction(0b000000000000000000, renderer, &mode);
    }

     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     SDL_Quit();
     return 0;
 }
