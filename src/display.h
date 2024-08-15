#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

typedef struct {
    bool running;
    uint32_t* io_buffer;
    SDL_Renderer* renderer;
    int mode;
} display_340;

void* run_display(void* display_arg);
void initialize_display(display_340* display, uint32_t* io_buffer);