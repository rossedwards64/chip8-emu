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

    printf("\r sp: 0x%04X, pc: 0x%04X, I: 0x%04X ", sp, pc, I);

    fflush(stdout);
}
#endif

// set up interpreter state, set memory and registers to zero
int init_emu(FILE *buffer, chip8_t *chip8)
{
    memset(&(chip8->display), 0, sizeof(chip8->display));
    memset(&(chip8->mem), 0, sizeof(chip8->mem));
    memset(&(chip8->v), 0, sizeof(chip8->v));
    memset(&(chip8->stack), 0, sizeof(chip8->stack));
    memset(&(chip8->key), 0, sizeof(chip8->key));
    chip8->I = 0;
    chip8->sp = 0;
    chip8->dt = 0;
    chip8->st = 0;
    chip8->pc = 0x200;

    for(uint8_t i = 0x0, j = 0x50; j <= 0x9F; ++i, ++j)
        chip8->mem[j] = fontset[i];
    SDL_Log("Loaded font into memory.\n");

    fread(&chip8->mem[0x200], 1, PROG_SIZE, buffer);

    SDL_Log("Loaded program into memory.\n");
    SDL_Log("Initialised interpreter.\n");

    return 0;
}

int parse_opcode(chip8_t *chip8)
{
    uint16_t cur_opcode = (chip8->mem[chip8->pc] << 8) | (chip8->mem[chip8->pc + 1]);
    chip8->pc += 2;

    const uint8_t p    = (cur_opcode >> 12) & 0x000F; /* 0xF000 - top 4 bits */
    const uint8_t x    = (cur_opcode >> 8)  & 0x000F; /* 0x0F00 - lower 4 bits of the top byte */
    const uint8_t y    = (cur_opcode >> 4)  & 0x000F; /* 0x00F0 - upper 4 bits of the lowest byte */
    const uint8_t n    = (cur_opcode)       & 0x000F; /* 0x000F - lowest 4 bits */
    const uint8_t kk   = (cur_opcode)       & 0x00FF; /* 0x00FF - lowest 8 bits */
    const uint16_t nnn = (cur_opcode)       & 0x0FFF; /* 0x0FFF - lowest 12 bits */
    uint8_t drawn = 0;

    switch(p) {
        case 0x0:
            switch(y) {
                case 0x0:
                    switch(kk) {
                        case 0xE0:
                            SDL_Log("0x%04X: CLEAR SCREEN\n", cur_opcode);
                            cls(chip8->display);
                            drawn = 1;
                            break;
                        case 0xEE:
                            SDL_Log("0x%04X: RETURN\n", cur_opcode);
                            break;
                    } break;
            } break;
        case 0x1:
            SDL_Log("0x%04X: JUMP TO 0x%04X\n", cur_opcode, nnn);
            chip8->pc -= 2;
            chip8->pc = nnn;
            break;
        case 0x2:
            SDL_Log("0x%04X: CALL 0x%04X\n", cur_opcode, nnn);
            break;
        case 0x3:
            SDL_Log("0x%04X: SKIP IF 0x%04X == 0x%04X\n", cur_opcode, chip8->v[x], kk);
            break;
        case 0x4:
            SDL_Log("0x%04X: SKIP IF 0x%04X != 0x%04X\n", cur_opcode, chip8->v[x], kk);
            break;
        case 0x5:
            SDL_Log("0x%04X: SKIP IF 0x%04X == 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
            break;
        case 0x6:
            SDL_Log("0x%04X: LOAD 0x%04X IN REGISTER 0x%04X\n", cur_opcode, kk, x);
            chip8->v[x] = kk;
            break;
        case 0x7:
            SDL_Log("0x%04X: ADD 0x%04X TO REGISTER 0x%04X\n", cur_opcode, kk, x);
            chip8->v[x] += kk;
            break;
        case 0x8:
            switch (n) {
                case 0x0:
                    SDL_Log("0x%04X: SET 0x%04X TO 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
                    break;
                case 0x1:
                    SDL_Log("0x%04X: 0x%04X OR 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
                    break;
                case 0x2:
                    SDL_Log("0x%04X: 0x%04X AND 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
                    break;
                case 0x3:
                    SDL_Log("0x%04X: 0x%04X XOR 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
                    break;
                case 0x4:
                    SDL_Log("0x%04X: 0x%04X ADD 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
                    break;
                case 0x5:
                    SDL_Log("0x%04X: 0x%04X SUB 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
                    break;
                case 0x6:
                    SDL_Log("0x%04X: SHIFT REGISTER 0x%04X RIGHT\n", cur_opcode, chip8->v[x]);
                    break;
                case 0x7:
                    SDL_Log("0x%04X: 0x%04X SUB 0x%04X\n", cur_opcode, chip8->v[y], chip8->v[x]);
                    break;
                case 0xE:
                    SDL_Log("0x%04X: SHIFT REGISTER 0x%04X LEFT\n", cur_opcode, chip8->v[x]);
                    break;
            }
            break;
        case 0x9:
            SDL_Log("0x%04X: SKIP IF 0x%04X != 0x%04X\n", cur_opcode, chip8->v[x], chip8->v[y]);
            break;
        case 0xA:
            SDL_Log("0x%04X: LOAD INSTR 0x%04X\n", cur_opcode, nnn);
            chip8->I = nnn;
            break;
        case 0xB:
            SDL_Log("0x%04X: JUMP TO ADDR 0x%04X\n", cur_opcode, chip8->v[0] + nnn);
            break;
        case 0xC:
            SDL_Log("0x%04X: SET REGISTER 0x%04X TO RAND NUM 0x%04X\n", cur_opcode, chip8->v[x], kk);
            break;
        case 0xD:
            SDL_Log("0x%04X: DRAW 0x%04X BYTES TO REGISTERS 0x%04X AND 0x%04X\n", cur_opcode, n, chip8->v[x], chip8->v[y]);
            draw(chip8->display, chip8->mem, chip8->I, chip8->v[x], chip8->v[y], n, &(chip8->v[0xF]));
            drawn = 1;
            break;
        case 0xE:
            switch(kk) {
                case 0x9E:
                    SDL_Log("0x%04X: SKIP IF 0x%04X IS PRESSED\n", cur_opcode, chip8->v[x]);
                    break;
                case 0xA1:
                    SDL_Log("0x%04X: SKIP IF 0x%04X IS NOT PRESSED\n", cur_opcode, chip8->v[x]);
                    break;
            }
            break;
        case 0xF:
            switch(kk) {
                case 0x07:
                    SDL_Log("0x%04X: SET REGISTER 0x%04X TO DELAY TIMER\n", cur_opcode, chip8->v[x]);
                    break;
                case 0x0A:
                    SDL_Log("0x%04X: WAIT FOR KEY\n", cur_opcode);
                    break;
                case 0x15:
                    SDL_Log("0x%04X: SET DELAY TIMER TO REGISTER 0x%04X", cur_opcode, chip8->v[x]);
                    break;
                case 0x18:
                    SDL_Log("0x%04X: SET SOUND TIMER TO 0x%04X\n", cur_opcode, chip8->v[x]);
                    break;
                case 0x1E:
                    SDL_Log("0x%04X: ADD INSTR 0x%04X AND REGISTER 0x%04X\n", cur_opcode, chip8->I, chip8->v[x]);
                    break;
                case 0x29:
                    SDL_Log("0x%04X: SET INSTR 0x%04X TO SPRITE 0x%04X\n", cur_opcode, chip8->I, chip8->v[x]);
                    break;
                case 0x33:
                    SDL_Log("0x%04X: SET DIGITS IN REGISTER 0x%04X TO INSTRS 0x%04X 0x%04X 0x%04X\n", cur_opcode, chip8->v[x], chip8->I, chip8->I + 1, chip8->I + 2);
                    break;
                case 0x55:
                    SDL_Log("0x%04X: LOAD ALL REGISTERS UP TO 0x%04X STARTING AT LOCATION 0x%04X\n", cur_opcode, chip8->v[x], chip8->I);
                    break;
                case 0x65:
                    SDL_Log("0x%04X: READ ALL REGISTERS UP TO 0x%04X STARTING AT LOCATION 0x%04X\n", cur_opcode, chip8->v[x], chip8->I);
                    break;
            }
            break;
        default:
            SDL_Log("Unknown opcode: 0x%04X\n", cur_opcode);
            break;
    }
    return drawn;
}

/* OPCODE FUNCTIONS */
void cls(bool display[DIS_ROWS][DIS_COLS])
{
    memset(display, 0, sizeof(*display));
}

void draw(bool display[DIS_ROWS][DIS_COLS], uint8_t mem[0x1000], uint16_t I, uint8_t v_x, uint8_t v_y, uint16_t n, uint8_t *coll_flag)
{
    uint16_t pixel;
    uint8_t y = 0, x = 0;

    for(uint16_t i = 0; i < n; ++i) { // rows
        if(y >= DIS_ROWS) break;
        pixel = mem[I + i];
        for(uint16_t j = 0; j < 8; ++j) {
            if(x >= DIS_COLS) break;
            y = (v_y + i) % DIS_ROWS;
            x = (v_x + j) % DIS_COLS;
            if((pixel & (0x80 >> j)) != 0) {
                if(display[y][x] == 1)
                    *coll_flag = 1;
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
