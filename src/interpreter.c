#include "interpreter.h"
#include <stdint.h>
#include <stdio.h>


#define WIDTH   64
#define HEIGHT  32
#define REFRESH 60

/* TODO: RAM should begin with 512 bytes for the interpreter (0x000 to 0x1FF),
 * then the rest is used for programs */
union memory {

};

/*  */
uint8_t  mem[0x1000] =             { 0 };    // 4KB of RAM
uint16_t stack[16] =               { 0 };    // can fit sixteen instructions on the stack at a time
uint8_t  sp =                      0x0;

/* REGISTERS */
uint8_t  v[16] =                  { 0 };    // registers go from v0 to vf, do not use vf
uint16_t pc =                     0x200;    // program memory starts at 0x200
uint16_t I =                      0x0;      // stores memory addresses

/* TIMERS */
uint8_t dt =                      0x0;
uint8_t st =                      0x0;

/* GRAPHICS */
uint8_t display[WIDTH * HEIGHT] = { 0 };
uint8_t fontset[80] =             { 0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
                                    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
                                    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
                                    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
                                    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
                                    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
                                    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
                                    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
                                    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
                                    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
                                    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
                                    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
                                    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
                                    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
                                    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
                                    0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F

// set up interpreter state, set memory and registers to zero
void init(FILE *buffer)
{
    for(uint8_t i = 0x0, j = 0x50; j <= 0x9F; ++i, ++j) {
        mem[j] = fontset[i];
    }

    uint16_t i = 0x200;
    uint16_t c;
    while(fread(&c, sizeof(uint16_t), 1, buffer)) {
        mem[i] = c & 0xFF;
        mem[i + 1] = (c >> 8) & 0xFF;
        i += 2;
    }
}

void fetch_opcode()
{

}

void parse_opcode()
{
    /*
    * 00E0 CLS
    * 1NNN JMP (address)
    * 6XNN LD  (v_x, address)
    * 7XNN ADD (v_x, address)
    * ANNN LD I (address)
    * DXYN DRW  (v_x, v_y, address) */
    uint16_t cur_opcode = (mem[pc] << 8) & 0xFF00 | (mem[pc + 1]) & 0x00FF;
    printf("Got opcode %X\n", cur_opcode);

    /* strip bitfields */
    uint8_t p    = (cur_opcode >> 12) & 0xF; /* 0xF000 */
    printf("instruction: %X\n", p);
    uint8_t y    = (cur_opcode >> 8) & 0xF; /* 0x0F00 */
    printf("v_y: register[%X]\n", y);
    uint8_t x    = (cur_opcode >> 4) & 0xF; /* 0x00F0 */
    printf("v_x: register[%X]\n", x);
    uint8_t kk   = (cur_opcode) & 0xFF;     /* 0x00FF */
    printf("kk: %X\n", kk);
    uint16_t nnn = (cur_opcode) & 0xFFF;    /* 0x0FFF */
    printf("nnn: %X\n", nnn);
    uint8_t n    = (cur_opcode) & 0xF;

    /* mask off the first number of the opcode */
    switch(p) {
        case 0x0:
            switch(y) {
                case 0x0:
                    switch(kk) {
                        case 0xE0:
                            printf("CLEAR SCREEN\n");
                            for(uint16_t i = 0x0; i <= (WIDTH * HEIGHT); ++i) /* CLS */
                                display[i] = 0x0;
                            pc += 0x2;
                            break;
                        case 0xEE:
                            printf("RETURN\n");
                            /*
                            ** RET: set the program counter to the address at the top of the stack and decrement the stack pointer
                             */
                            break;
                    } break;
            } break;
        case 0x1:
            printf("JUMP TO %X\n", nnn);
            pc = nnn; /* JMP */
            break;
        case 0x2:

            break;
        case 0x3:

            break;
        case 0x4:

            break;
        case 0x5:

            break;
        case 0x6:
            printf("LOAD %X IN REGISTER %X\n", kk, x);
            v[(cur_opcode & 0x0F00) >> 8] = cur_opcode & 0x00FF; /* LD */
            pc += 0x2;
            break;
        case 0x7:
            printf("ADD %X TO REGISTER %X\n", kk, x);
            v[(cur_opcode & 0x0F00) >> 8] += cur_opcode & 0x00FF; /* ADD */
            pc += 0x2;
            break;
        case 0x8:

            break;
        case 0x9:

            break;
        case 0xA:
            printf("LOAD INSTRUCTION %X\n", nnn);
            I = (cur_opcode & 0x0FFF) + v[0]; /* LD I */
            pc += 0x2;
            break;
        case 0xB:

            break;
        case 0xC:

            break;
        case 0xD:
        {
            printf("DRAW %X BYTES TO REGISTERS %X AND %X\n", n, y, x);
            uint16_t sprites[n];
            uint16_t addr = I;
            for(uint8_t i = 0; i < n; i++) {
                sprites[i] = addr;
                addr++;
                display[v[x + i] * v[y + i]] ^= sprites[i];
            }
        }
            break;
        case 0xE:

            break;
        case 0xF:

            break;
        default:
            printf("Unknown opcode: %X\n", cur_opcode);
            break;
    }
}

/* OPCODE FUNCTIONS */
void cls()
{

}

void draw(uint8_t v_x, uint8_t v_y, uint8_t addr)
{

}

void update_delay(uint8_t v_x)
{
    dt = v_x;
}

void update_sound(uint8_t v_x)
{
    st = v_x;
}

void dump_register(uint8_t v_x, uint8_t *instr)
{

}

void load_register(uint8_t v_x, uint8_t *instr)
{

}
