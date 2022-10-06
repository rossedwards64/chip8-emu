#ifndef GFX_H_
#define GFX_H_

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

int init_sdl();
void update();
void render();
void handle_inputs(bool *quit);
void close_sdl();

#endif // GFX_H_
