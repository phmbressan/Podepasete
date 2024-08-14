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
} teleprinter_340;

void* run_teleprinter(void* teleprinter_arg);
void initialize_teleprinter(teleprinter_340* teleprinter, uint32_t* io_buffer);