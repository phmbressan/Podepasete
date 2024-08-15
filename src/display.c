#include "display.h"
#include <stdio.h>
#include <unistd.h>

const int FONT_SIZE = 2;  
const int FONT_WIDTH = 5;
const int FONT_HEIGHT = 7;
const int X_SEP = 1;
const int Y_SEP = 1; 

const uint8_t FONT_MAP[64][7] = {
    [0]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '\0'
    [1]  = {0x7e, 0x11, 0x11, 0x11, 0x7e, 0x00, 0x00}, // 'A'
    [2]  = {0x7f, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00}, // 'B'
    [3]  = {0x3e, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00}, // 'C'
    [4]  = {0x7f, 0x41, 0x41, 0x22, 0x1c, 0x00, 0x00}, // 'D'
    [5]  = {0x7f, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00}, // 'E'
    [6]  = {0x7f, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00}, // 'F'
    [7]  = {0x3e, 0x41, 0x49, 0x49, 0x7a, 0x00, 0x00}, // 'G'
    [8]  = {0x7f, 0x08, 0x08, 0x08, 0x7f, 0x00, 0x00}, // 'H'
    [9]  = {0x00, 0x41, 0x7f, 0x41, 0x00, 0x00, 0x00}, // 'I'
    [10] = {0x20, 0x40, 0x41, 0x3f, 0x01, 0x00, 0x00}, // 'J'
    [11] = {0x7f, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00}, // 'K'
    [12] = {0x7f, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00}, // 'L'
    [13] = {0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00, 0x00}, // 'M'
    [14] = {0x7f, 0x04, 0x08, 0x10, 0x7f, 0x00, 0x00}, // 'N'
    [15] = {0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00}, // 'O'
    [16] = {0x7f, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00}, // 'P'
    [17] = {0x3e, 0x41, 0x51, 0x21, 0x5e, 0x00, 0x00}, // 'Q'
    [18] = {0x7f, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00}, // 'R'
    [19] = {0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00}, // 'S'
    [20] = {0x01, 0x01, 0x7f, 0x01, 0x01, 0x00, 0x00}, // 'T'
    [21] = {0x3f, 0x40, 0x40, 0x40, 0x3f, 0x00, 0x00}, // 'U'
    [22] = {0x1f, 0x20, 0x40, 0x20, 0x1f, 0x00, 0x00}, // 'V'
    [23] = {0x3f, 0x40, 0x38, 0x40, 0x3f, 0x00, 0x00}, // 'W'
    [24] = {0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00}, // 'X'
    [25] = {0x07, 0x08, 0x70, 0x08, 0x07, 0x00, 0x00}, // 'Y'
    [26] = {0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00}, // 'Z'
    [27] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '\n'
    [28] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '\r'
    [29] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'shift h'
    [30] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'shift v'
    [31] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'ESC'
    [32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ' '
	[33] = {0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00}, // '!'
	[34] = {0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00}, // '"'
	[35] = {0x14, 0x7f, 0x14, 0x7f, 0x14, 0x00, 0x00}, // '#'
	[36] = {0x24, 0x2a, 0x7f, 0x2a, 0x12, 0x00, 0x00}, // '$'
	[37] = {0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00}, // '%'
	[38] = {0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x00}, // '&'
	[39] = {0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00}, // '''
	[40] = {0x00, 0x1c, 0x22, 0x41, 0x00, 0x00, 0x00}, // '('
	[41] = {0x00, 0x41, 0x22, 0x1c, 0x00, 0x00, 0x00}, // ')'
	[42] = {0x14, 0x08, 0x3e, 0x08, 0x14, 0x00, 0x00}, // '*'
	[43] = {0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00}, // '+'
	[44] = {0x00, 0x50, 0x30, 0x00, 0x00, 0x00, 0x00}, // ','
	[45] = {0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00}, // '-'
	[46] = {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00}, // '.'
	[47] = {0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00}, // '/'
    [48] = {0x3e, 0x51, 0x49, 0x45, 0x3e, 0x00, 0x00}, // '0'
    [49] = {0x00, 0x42, 0x7f, 0x40, 0x00, 0x00, 0x00}, // '1'
    [50] = {0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00}, // '2'
    [51] = {0x21, 0x41, 0x45, 0x4b, 0x31, 0x00, 0x00}, // '3'
    [52] = {0x18, 0x14, 0x12, 0x7f, 0x10, 0x00, 0x00}, // '4'
    [53] = {0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00}, // '5'
    [54] = {0x3c, 0x4a, 0x49, 0x49, 0x30, 0x00, 0x00}, // '6'
    [55] = {0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00}, // '7'
    [56] = {0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00}, // '8'
    [57] = {0x06, 0x49, 0x49, 0x29, 0x1e, 0x00, 0x00}, // '9'
    [58] = {0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00}, // ':'
    [59] = {0x00, 0x56, 0x36, 0x00, 0x00, 0x00, 0x00}, // ';'
    [60] = {0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00}, // '<'
    [61] = {0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00}, // '='
    [62] = {0x00, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00}, // '>'
    [63] = {0x02, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00}, // '?'
};

SDL_Renderer* start_SDL_renderer(void);
void set_pixel(int x, int y, SDL_Renderer *renderer);
void draw_char(int x, int y, uint8_t char_code, SDL_Renderer *renderer);
void handle_instruction(uint32_t instruction, SDL_Renderer *renderer, int *mode);
void print_display(display_340* display);
uint8_t int_to_char(uint8_t single);

void* run_display(void* display_arg) {
    display_340* display = (display_340*) display_arg;
    
    SDL_Event event;
    bool quit = false;

    while (!quit && display->running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        if (*display->io_buffer == 0) { continue; }

        print_display(display);
        SDL_RenderPresent(display->renderer);
        *display->io_buffer = 0;
        usleep(35000);
        display->mode = 0;
    }

    SDL_DestroyRenderer(display->renderer);
    SDL_Quit();
    return NULL;
 }

void initialize_display(display_340* display, uint32_t* io_buffer) {
    SDL_Renderer *renderer = start_SDL_renderer();
    display->running = true;
    display->io_buffer = io_buffer;   
    display->renderer = renderer;
    display->mode = 0;
}

SDL_Renderer* start_SDL_renderer(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow("340 Display", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    return renderer;
}

void handle_instruction(uint32_t instruction, SDL_Renderer *renderer, int *mode) {
    switch (*mode) {
        case 0: // Parameter mode
            *mode = (instruction >> 13) & 0x7;
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
            } else {
                SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT - coord);
            }
            *mode = next_mode;
        }
        break;
        case 3: // Char mode
            uint32_t curr_inst = instruction;
            uint8_t curr_char;
            static int x_delta = 0, y_delta = 0;

            for (int i = 1; i <= 3; i++) {
                curr_char = (curr_inst & 0x3F000) >> 12;
                curr_inst = curr_inst << 6;
       
                switch (curr_char) {
                    case 0: // Null character
                        continue;
                    case 27: // \n
                        y_delta += (FONT_HEIGHT + Y_SEP) * FONT_SIZE;
                        break;
                    case 28: // \r
                        x_delta = 0;
                        break;
                    case 29: // Shift h
                        x_delta += (FONT_WIDTH + X_SEP) * FONT_SIZE;
                        break;
                    case 30: // Shift v
                        y_delta += (FONT_HEIGHT + Y_SEP) * FONT_SIZE;
                        break;
                    case 31: // Escape
                        *mode = 0;
                        break;
                    default: // Valid char
                        draw_char(x_delta, y_delta, curr_char, renderer);
                        x_delta += (FONT_WIDTH + X_SEP) * FONT_SIZE;
                        if (x_delta >= SCREEN_WIDTH - FONT_SIZE * FONT_WIDTH) {
                            x_delta = 0;
                            y_delta += (FONT_HEIGHT + Y_SEP) * FONT_SIZE;
                        }
                        break;
                }
            }

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
            if (!stay_in_mode) {
                *mode = 0;
            }
        }
        break;
        default:
            *mode = 0;
            break;
    }
}

void print_display(display_340* display) {
    display->mode = 3;

    uint32_t number = *display->io_buffer;
    uint32_t instruction = 0;
    uint8_t len_number = 0;

    while (number != 0) {
        number /= 8;
        len_number++;
    }

    uint8_t* number_array = (uint8_t*) malloc(len_number * sizeof(uint8_t));

    number = *display->io_buffer;

    for (int i = 0; i < len_number; i++) {
        number_array[i] = number % 8;
        number /= 8;
    }

    for (int i = len_number - 1, j = 0; i >= 0; i--, j++) {
        uint8_t curr_char = int_to_char(number_array[i]);
        instruction = (instruction << 6) | (curr_char);  
    
        if (j == 2) {
            handle_instruction(instruction, display->renderer, &display->mode);
            instruction = 0;
            j = -1;
        }
    }

    if (len_number % 3 != 0) {
        int remaining = 3 - (len_number % 3);
        while (remaining != 0) {
            instruction = (instruction << 6) | 0;
            remaining--;
        }
        handle_instruction(instruction, display->renderer, &display->mode);
    }

    free(number_array);

    instruction = 0b011011011100000000; // \r\n\0
    handle_instruction(instruction, display->renderer, &display->mode);
}

uint8_t int_to_char(uint8_t single){
    if (single < 10) {
        return single + 48;
    } 
    printf("Invalid number: %d\n", single);
    return 0;
}

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
