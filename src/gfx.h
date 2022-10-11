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

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define WIDTH 64
#define HEIGHT 32
#define BLACK 0
#define WHITE 0

int init_sdl();
void update();
void render(bool display[WIDTH][HEIGHT]);
void handle_inputs(bool *quit);
void close_sdl();

#endif // GFX_H_
