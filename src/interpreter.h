#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <SDL2/SDL_log.h>

#include "util.h"


typedef struct {
        uint8_t mem[MEM_SIZE];    // 4KB of RAM

        uint16_t stack[ARR_SIZE]; // can fit sixteen instructions on the stack at a time
        uint8_t sp;

        /* REGISTERS */
        uint8_t v[ARR_SIZE];      // registers go from v0 to vf
        uint16_t pc;              // program memory starts at 0x200
        uint16_t I;               // stores memory addresses

        /* TIMERS */
        uint8_t dt;
        uint8_t st;

        /* GRAPHICS */
        bool display[DIS_ROWS][DIS_COLS];

        bool key[ARR_SIZE];
} chip8_t;

#ifdef DEBUG
void print_mem(chip8_t *chip8);
void print_reg(chip8_t *chip8);
#endif

uint8_t init_emu(chip8_t *chip8, FILE *program);
bool execute_opcode(chip8_t *chip8);
void update_timers(chip8_t *chip8);

#endif // INTERPRETER_H_
