#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

void set_pixel(int x, int y, SDL_Renderer *renderer);
void draw_char(int x, int y, uint8_t char_code, SDL_Renderer *renderer);
void handle_instruction(uint32_t instruction, SDL_Renderer *renderer, int *mode);