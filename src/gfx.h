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


int init_sdl();
void render(bool display[DIS_HEIGHT][DIS_WIDTH]);
void handle_inputs(bool *quit);
void close_sdl();

#endif // GFX_H_
