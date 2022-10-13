#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL_log.h>

#include "util.h"


typedef struct chip_8 {
    union {
        uint8_t mem[0x1000]; // 4KB of RAM

        struct {
            uint16_t stack[16];      // can fit sixteen instructions on the stack at a time
            uint8_t sp;

            /* REGISTERS */
            uint8_t v[16];           // registers go from v0 to vf, do not use vf
            uint16_t pc;             // program memory starts at 0x200
            uint16_t I;              // stores memory addresses

            /* TIMERS */
            uint8_t dt;
            uint8_t st;

            /* GRAPHICS */
            bool display[DIS_HEIGHT][DIS_WIDTH];
        };
    };
} chip8_t;


int init_emu(FILE *buffer, chip8_t *chip8);
void parse_opcode(chip8_t *chip8);

/* OPCODE FUNCTIONS */
void cls(bool display[DIS_HEIGHT][DIS_WIDTH]);
void draw(bool display[DIS_HEIGHT][DIS_WIDTH], uint16_t sprites[], uint8_t v_x, uint8_t v_y, uint16_t n, uint16_t I);
void update_delay(uint8_t v_x, uint8_t *dt);
void update_sound(uint8_t v_x, uint8_t *st);
/* void dump_register(uint8_t v_x, uint8_t *instr); */
/* void load_register(uint8_t v_x, uint8_t *instr); */

#endif // INTERPRETER_H_
