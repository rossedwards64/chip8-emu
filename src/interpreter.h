#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

#define WIDTH  64
#define HEIGHT 32
#define REFRESH 60

int init_sdl();
void update();
void render();
void handle_inputs(bool *quit);
void close_sdl();


void init_emu(FILE *buffer);
void fetch_opcode();
void parse_opcode();

/* OPCODE FUNCTIONS */
void cls();
void draw(uint8_t v_x, uint8_t v_y, uint8_t addr);
void update_delay(uint8_t v_x);
void update_sound(uint8_t v_x);
void dump_register(uint8_t v_x, uint8_t *instr);
void load_register(uint8_t v_x, uint8_t *instr);

#endif // INTERPRETER_H_
