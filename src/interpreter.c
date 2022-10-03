#include "interpreter.h"


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
uint16_t instr =                  0x0;      // stores memory addresses

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
    for(uint8_t i = 0x50; i <= 0x9F; ++i)
        mem[i] = fontset[i];

    uint16_t i = 0x200;
    uint16_t c;
    while((c = fgetc(buffer)) != feof(buffer))
    {
        mem[i] = c;
        ++i;
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
    uint16_t cur_opcode = mem[pc] << 8 | mem[pc + 1];
    /* mask off the first number of the opcode */
    switch(cur_opcode & 0xF000) {
        case 0x0000:
            switch(cur_opcode & 0x0F00) {
                case 0x0E00:
                    switch(cur_opcode & 0x00F) {
                        case 0x0E0:
                            for(uint16_t i = 0x0; i <= (WIDTH * HEIGHT); ++i) /* CLS */
                                display[i] = 0x0;
                            pc += 0x2;
                            break;
                        case 0x00EE:
                            /*
                            ** RET: set the program counter to the address at the top of the stack and decrement the stack pointer
                             */
                            break;
                    } break;
            } break;
        case 0x1000:
            pc = (cur_opcode & 0x0FFF); /* JMP */
            break;
        case 0x2000:

            break;
        case 0x3000:

            break;
        case 0x4000:

            break;
        case 0x5000:

            break;
        case 0x6000:
            v[(cur_opcode & 0x0F00) >> 8] = cur_opcode & 0x00FF; /* LD */
            pc += 0x2;
            break;
        case 0x7000:
            v[(cur_opcode & 0x0F00) >> 8] += cur_opcode & 0x00FF; /* ADD */
            pc += 0x2;
            break;
        case 0x8000:

            break;
        case 0x9000:

            break;
        case 0xA000:
            instr = (cur_opcode & 0x0FFF) + v[0]; /* LD I */
            pc += 0x2;
            break;
        case 0xB000:

            break;
        case 0xC000:

            break;
        case 0xD000:

            break;
        case 0xE000:

            break;
        case 0xF000:

            break;
    }
}
        /* case 0x00E0: */
        /*     break; */

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
