#ifndef GFX_H_
#define GFX_H_

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include "util.h"


uint8_t init_sdl();
void print(bool display[DIS_ROWS][DIS_COLS]);
void render(bool display[DIS_ROWS][DIS_COLS]);
void handle_inputs(bool *quit, bool key[ARR_SIZE]);
void close_sdl();

#endif // GFX_H_
