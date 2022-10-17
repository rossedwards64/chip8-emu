#include "interpreter.h"
#include "util.h"


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

#ifdef DEBUG
void print_mem(chip8_t *chip8)
{
    for(uint16_t i = 0x0; i <= 0x1000; i += 2) {
        printf("%.2x", chip8->mem[i]);
        printf("%.2x ", chip8->mem[i + 1]);
    }
    printf("\n");
}

void print_op(chip8_t *chip8)
{
    uint16_t sp = chip8->sp;
    uint16_t pc = chip8->pc;
    uint16_t I = chip8->I;

    printf("\r sp: 0x%02x, pc: 0x%02x, I: 0x%02x ", sp, pc, I);

    fflush(stdout);
}
#endif

// set up interpreter state, set memory and registers to zero
int init_emu(FILE *buffer, chip8_t *chip8)
{
    memset(&chip8->mem, 0, sizeof(chip8->mem));
    memset(&chip8->v, 0, sizeof(chip8->v));
    memset(&chip8->stack, 0, sizeof(chip8->stack));
    memset(&chip8->key, 0, sizeof(chip8->key));
    chip8->I = 0;
    chip8->sp = 0;
    chip8->dt = 0;
    chip8->st = 0;
    chip8->pc = 0x200;

    for(uint8_t i = 0x0, j = 0x50; j <= 0x9F; ++i, ++j)
        chip8->mem[j] = fontset[i];
    SDL_Log("Loaded font into memory.\n");

    uint16_t i = chip8->pc;
    uint16_t c;
    while(fread(&c, sizeof(uint16_t), 1, buffer) > 0) {
        chip8->mem[i] = c << 8;
        chip8->mem[i + 1] = c;
        i += 2;
    }
    SDL_Log("Loaded program into memory.\n");
    SDL_Log("Initialised interpreter.\n");

    return 0;
}

void parse_opcode(chip8_t *chip8)
{
    uint16_t cur_opcode = (chip8->mem[chip8->pc] << 8) | (chip8->mem[chip8->pc + 1]);
    /* SDL_Log("Got opcode 0x%02X\n", cur_opcode); */
    chip8->pc += 2;

    uint8_t p    = (cur_opcode >> 12);    /* 0xF000 - top 4 bits */
    uint8_t x    = (cur_opcode >> 8);     /* 0x0F00 - lower 4 bits of the top byte */
    uint8_t y    = (cur_opcode >> 4);     /* 0x00F0 - upper 4 bits of the lowest byte */
    uint8_t n    = (cur_opcode) & 0x000F; /* 0x000F - lowest 4 bits */
    uint8_t kk   = (cur_opcode) & 0x00FF; /* 0x00FF - lowest 8 bits */
    uint16_t nnn = (cur_opcode) & 0x0FFF; /* 0x0FFF - lowest 12 bits */

    switch(p) {
        case 0x0:
            switch(y) {
                case 0x0:
                    switch(kk) {
                        case 0xE0:
                            cls(chip8->display);
                            break;
                        case 0xEE:
                            SDL_Log("RETURN\n");
                            break;
                    } break;
            } break;
        case 0x1:
            SDL_Log("JUMP TO 0x%02X\n", nnn);
            chip8->pc = nnn;
            break;
        case 0x2:
            SDL_Log("CALL 0x%02X\n", nnn);
            break;
        case 0x3:
            SDL_Log("SKIP IF 0x%02X == 0x%02X\n", chip8->v[x], kk);
            break;
        case 0x4:
            SDL_Log("SKIP IF 0x%02X != 0x%02X\n", chip8->v[x], kk);
            break;
        case 0x5:
            SDL_Log("SKIP IF 0x%02X == 0x%02X\n", chip8->v[x], chip8->v[y]);
            break;
        case 0x6:
            SDL_Log("LOAD 0x%02X IN REGISTER 0x%02X\n", kk, x);
            chip8->v[x] = kk;
            break;
        case 0x7:
            SDL_Log("ADD 0x%02X TO REGISTER 0x%02X\n", kk, x);
            chip8->v[x] += kk;
            break;
        case 0x8:
            switch (n) {
                case 0x0:
                    SDL_Log("SET 0x%02X TO 0x%02X\n", chip8->v[x], chip8->v[y]);
                    break;
                case 0x1:
                    SDL_Log("0x%02X OR 0x%02X\n", chip8->v[x], chip8->v[y]);
                    break;
                case 0x2:
                    SDL_Log("0x%02X AND 0x%02X\n", chip8->v[x], chip8->v[y]);
                    break;
                case 0x3:
                    SDL_Log("0x%02X XOR 0x%02X\n", chip8->v[x], chip8->v[y]);
                    break;
                case 0x4:
                    SDL_Log("0x%02X ADD 0x%02X\n", chip8->v[x], chip8->v[y]);
                    break;
                case 0x5:
                    SDL_Log("0x%02X SUB 0x%02X\n", chip8->v[x], chip8->v[y]);
                    break;
                case 0x6:
                    SDL_Log("SHIFT REGISTER 0x%02X RIGHT\n", chip8->v[x]);
                    break;
                case 0x7:
                    SDL_Log("0x%02X SUB 0x%02X\n", chip8->v[y], chip8->v[x]);
                    break;
                case 0xE:
                    SDL_Log("SHIFT REGISTER 0x%02X LEFT\n", chip8->v[x]);
                    break;
            }
            break;
        case 0x9:
            SDL_Log("SKIP IF 0x%02X != 0x%02X\n", chip8->v[x], chip8->v[y]);
            break;
        case 0xA:
            SDL_Log("LOAD INSTR 0x%02X\n", nnn);
            chip8->I = nnn;
            break;
        case 0xB:
            SDL_Log("JUMP TO ADDR 0x%02X\n", chip8->v[0] + nnn);
            break;
        case 0xC:
            SDL_Log("SET REGISTER 0x%02X TO RAND NUM 0x%02X\n", chip8->v[x], kk);
            break;
        case 0xD:
            draw(chip8->display, chip8->mem, chip8->I, chip8->v[x], chip8->v[y], n);
            break;
        case 0xE:
            switch(kk) {
                case 0x9E:
                    SDL_Log("SKIP IF 0x%02X IS PRESSED\n", chip8->v[x]);
                    break;
                case 0xA1:
                    SDL_Log("SKIP IF 0x%02X IS NOT PRESSED\n", chip8->v[8]);
                    break;
            }
            break;
        case 0xF:
            switch(kk) {
                case 0x07:
                    SDL_Log("SET REGISTER 0x%02X TO DELAY TIMER\n", chip8->v[x]);
                    break;
                case 0x0A:
                    SDL_Log("WAIT FOR KEY\n");
                    break;
                case 0x15:
                    SDL_Log("SET DELAY TIMER TO REGISTER 0x%02X", chip8->v[x]);
                    break;
                case 0x18:
                    SDL_Log("SET SOUND TIMER TO 0x%02X\n", chip8->v[x]);
                    break;
                case 0x1E:
                    SDL_Log("ADD INSTR 0x%02X AND REGISTER 0x%02X\n", chip8->I, chip8->v[x]);
                    break;
                case 0x29:
                    SDL_Log("SET INSTR 0x%02X TO SPRITE 0x%02X\n", chip8->I, chip8->v[x]);
                    break;
                case 0x33:
                    SDL_Log("SET DIGITS IN REGISTER 0x%02X TO INSTRS 0x%02X 0x%02X 0x%02X\n", chip8->v[x], chip8->I, chip8->I + 1, chip8->I + 2);
                    break;
                case 0x55:
                    SDL_Log("LOAD ALL REGISTERS UP TO 0x%02X STARTING AT LOCATION 0x%02X\n", chip8->v[x], chip8->I);
                    break;
                case 0x65:
                    SDL_Log("READ ALL REGISTERS UP TO 0x%02X STARTING AT LOCATION 0x%02X\n", chip8->v[x], chip8->I);
                    break;
            }
            break;
        default:
            SDL_Log("Unknown opcode: 0x%02X\n", cur_opcode);
            break;
    }
}

/* OPCODE FUNCTIONS */
void cls(bool (*display)[DIS_COLS])
{
    SDL_Log("CLEAR SCREEN\n");
    memset(display, 0, sizeof(display[0][0]) * DIS_ROWS * DIS_COLS);
}

void draw(bool (*display)[DIS_COLS], uint8_t mem[0x1000], uint16_t I, uint8_t v_x, uint8_t v_y, uint16_t n)
{
    SDL_Log("DRAW 0x%02X BYTES TO REGISTERS 0x%02X AND 0x%02X\n", DIS_ROWS * DIS_COLS, v_x, v_y);
    uint16_t pixel;

    for(uint16_t i = 0; i < n; i++) { // rows
        pixel = mem[I + i];
        for(uint16_t j = 0; j < 8; j++) { // cols
            uint8_t x = (v_x + i) % DIS_ROWS;
            uint8_t y = (v_y + j) % DIS_COLS;
            if((pixel & (0x80 >> j)) != 0) {
                display[y][x] ^= 1;
            }
        }
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
