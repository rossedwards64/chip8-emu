#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <SDL2/SDL_log.h>

#include "util.h"


typedef struct chip_8 {
        uint8_t mem[MEM_SIZE]; // 4KB of RAM

        uint16_t stack[16];    // can fit sixteen instructions on the stack at a time
        uint8_t sp;

        /* REGISTERS */
        uint8_t v[16];         // registers go from v0 to vf
        uint16_t pc;           // program memory starts at 0x200
        uint16_t I;            // stores memory addresses

        /* TIMERS */
        uint8_t dt;
        uint8_t st;

        /* GRAPHICS */
        bool display[DIS_ROWS][DIS_COLS];

        bool key[16];
} chip8_t;

#ifdef DEBUG
void print_mem(chip8_t *chip8);
void print_op(chip8_t *chip8);
#endif
int init_emu(FILE *buffer, chip8_t *chip8);
uint8_t parse_opcode(chip8_t *chip8);

/* OPCODE FUNCTIONS */
void cls(bool (*display)[DIS_ROWS][DIS_COLS]);
void draw(bool display[DIS_ROWS][DIS_COLS], uint8_t mem[MEM_SIZE], uint16_t I, uint8_t v_x, uint8_t v_y, uint16_t n, uint8_t *coll_flag);
void update_delay(uint8_t *dt, uint8_t v_x);
void update_sound(uint8_t *st, uint8_t v_x);
/* void dump_register(uint8_t v_x, uint8_t *instr); */
/* void load_register(uint8_t v_x, uint8_t *instr); */

#endif // INTERPRETER_H_
