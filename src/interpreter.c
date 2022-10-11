#include "interpreter.h"


uint8_t fontset[80] = { 0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
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
int init_emu(FILE *buffer, chip8_t *chip8)
{
    for(uint8_t i = 0x0, j = 0x50; j <= 0x9F; ++i, ++j) {
        chip8->mem[j] = fontset[i];
    }
    SDL_Log("Loaded font into memory.\n");

    uint16_t i = 0x200;
    uint16_t c;
    while(fread(&c, sizeof(uint16_t), 1, buffer) > 0) {
        chip8->mem[i] = c & 0xFF;
        chip8->mem[i + 1] = (c >> 8) & 0xFF;
        i += 2;
    }
    SDL_Log("Loaded program into memory.\n");
    SDL_Log("Initialised interpreter.\n");
    return 0;
}

void parse_opcode(chip8_t *chip8)
{
    /*
    * 00E0 CLS
    * 1NNN JMP (address)
    * 6XNN LD  (v_x, address)
    * 7XNN ADD (v_x, address)
    * ANNN LD I (address)
    * DXYN DRW  (v_x, v_y, address) */
    uint16_t cur_opcode = ((chip8->mem[chip8->pc] << 8) & 0xFF00) | ((chip8->mem[chip8->pc + 1]) & 0x00FF);
    //SDL_Log("Got opcode %X\n", cur_opcode);

    /* strip bitfields */
    uint8_t p    = (cur_opcode >> 12) & 0xF; /* 0xF000 */
    //SDL_Log("instruction: %X\n", p);
    uint8_t y    = (cur_opcode >> 8) & 0xF; /* 0x0F00 */
    //SDL_Log("v_y: register[%X]\n", y);
    uint8_t x    = (cur_opcode >> 4) & 0xF; /* 0x00F0 */
    //SDL_Log("v_x: register[%X]\n", x);
    uint8_t n    = (cur_opcode) & 0xF;      /* 0x000F */
    uint8_t kk   = (cur_opcode) & 0xFF;     /* 0x00FF */
    //SDL_Log("kk: %X\n", kk);
    uint16_t nnn = (cur_opcode) & 0xFFF;    /* 0x0FFF */
    //SDL_Log("nnn: %X\n", nnn);

    /* mask off the first number of the opcode */
    switch(p) {
        case 0x0:
            switch(y) {
                case 0x0:
                    switch(kk) {
                        case 0xE0:
                            cls(chip8->display);
                            chip8->pc += 0x2;
                            break;
                        case 0xEE:
                            //SDL_Log("RETURN\n");
                            /*
                            ** RET: set the program counter to the address at the top of the stack and decrement the stack pointer
                             */
                            break;
                    } break;
            } break;
        case 0x1:
            //SDL_Log("JUMP TO %X\n", nnn);
            chip8->pc = nnn; /* JMP */
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
            //SDL_Log("LOAD %X IN REGISTER %X\n", kk, x);
            chip8->v[(cur_opcode & 0x0F00) >> 8] = cur_opcode & 0x00FF; /* LD */
            chip8->pc += 0x2;
            break;
        case 0x7:
            //SDL_Log("ADD %X TO REGISTER %X\n", kk, x);
            chip8->v[(cur_opcode & 0x0F00) >> 8] += cur_opcode & 0x00FF; /* ADD */
            chip8->pc += 0x2;
            break;
        case 0x8:

            break;
        case 0x9:

            break;
        case 0xA:
            //SDL_Log("LOAD INSTRUCTION %X\n", nnn);
            chip8->I = (cur_opcode & 0x0FFF) + chip8->v[0]; /* LD I */
            chip8->pc += 0x2;
            break;
        case 0xB:

            break;
        case 0xC:

            break;
        case 0xD:
            {
                uint16_t sprites[n];
                draw(chip8->display, sprites, x, y, chip8->I);
            }
            break;
        case 0xE:

            break;
        case 0xF:

            break;
        default:
            SDL_Log("Unknown opcode: %X\n", cur_opcode);
            break;
    }
}

/* OPCODE FUNCTIONS */
void cls(bool display[WIDTH][HEIGHT])
{
    //SDL_Log("CLEAR SCREEN\n");
    for(uint16_t i = 0x0; i <= (WIDTH * HEIGHT); ++i) /* CLS */
        display[i][i] = 0x0;
}

void draw(bool display[WIDTH][HEIGHT], uint16_t sprites[], uint8_t v_x, uint8_t v_y, uint8_t addr)
{
    //SDL_Log("DRAW %X BYTES TO REGISTERS %X AND %X\n", n, y, x);
    for(uint16_t i = 0; i < sizeof(*sprites); i++) {
        sprites[i] = addr;
        addr++;
        display[v_x + i][v_y + i] ^= sprites[i];
    }

}

void update_delay(uint8_t v_x, uint8_t *dt)
{
    *dt = v_x;
}

void update_sound(uint8_t v_x, uint8_t *st)
{
    *st = v_x;
}

/* void dump_register(uint8_t v_x, uint8_t *instr) */
/* { */

/* } */

/* void load_register(uint8_t v_x, uint8_t *instr) */
/* { */

/* } */
