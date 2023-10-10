#include "interpreter.h"
#include "util.h"
#include <SDL2/SDL_log.h>
#include <stdint.h>

#undef DEBUG

static uint8_t fontset[80] = {  0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
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
                                0xF0, 0x80, 0xF0, 0x80, 0x80    // F
};

#ifdef DEBUG
void print_mem(chip8_t *chip8)
{
    for(uint16_t i = 0x0; i <= MEM_SIZE; i += 2) {
        printf("0x%04X", chip8->mem[i]);
        printf("0x%04X ", chip8->mem[i + 1]);
    }
    printf("\n");
}

void print_reg(chip8_t *chip8)
{
    printf("\r");
    for(uint8_t i = 0; i < ARR_SIZE; i++)
        printf("v%x: 0x%04X ", i, chip8->v[i]);
    fflush(stdout);
}
#endif

/* OPCODE FUNCTIONS */
static bool get_key(chip8_t *chip8, uint8_t x)
{
    bool key_pressed = false;
    for (uint8_t i = 0; i < ARR_SIZE && !key_pressed; i++) {
        if(chip8->key[i] == true) {
            key_pressed = true;
            chip8->v[x] = chip8->key[i];
        }
    }
    return key_pressed;
}

static void add_and_check_carry(chip8_t *chip8, uint8_t x, uint8_t y)
{
    uint16_t result = chip8->v[x] + chip8->v[y];
    chip8->v[x] = result & 0xFF;
    if(result > 255)
        chip8->v[0xF] = 1;
    else
        chip8->v[0xF] = 0;
}

static void draw(chip8_t *chip8, uint8_t v_x, uint8_t v_y, uint16_t n)
{
    uint16_t pixel = 0;
    uint8_t pos_y = 0, pos_x = 0;

    for(uint16_t i = 0; i < n; ++i) {
        if(pos_y >= DIS_ROWS) break;
        pixel = chip8->mem[chip8->I + i];
        pos_y = (chip8->v[v_y] + i) % DIS_ROWS;
        for(uint8_t j = 0; j < 8; ++j) {
            if(pos_x >= DIS_COLS) break;
            pos_x = (chip8->v[v_x] + j) % DIS_COLS;
            if((pixel & (0x80 >> j)) != 0) {
                if(chip8->display[pos_y][pos_x] == 1)
                    chip8->v[0xF] = 1;
                chip8->display[pos_y][pos_x] ^= 1;
            }
        }
    }
}

static void convert_decimal(chip8_t *chip8, uint8_t x)
{
    chip8->mem[chip8->I] = chip8->v[x] / 100;
    chip8->mem[chip8->I + 1] = (chip8->v[x] % 100) / 10;
    chip8->mem[chip8->I + 2] = chip8->v[x] % 10;
}

uint8_t init_emu(chip8_t *chip8, FILE *program)
{
    srand(0);
    *chip8 = (chip8_t){ 0 };
    chip8->pc = PROG_START;

    for(uint8_t i = 0x0, j = 0x50; j <= 0x9F; ++i, ++j)
        chip8->mem[j] = fontset[i];
    SDL_Log("Loaded font into memory.");

    fseek(program, 0, SEEK_END);
    uint16_t file_size = ftell(program);
    rewind(program);

    if(file_size < PROG_SIZE) {
        fread(&chip8->mem[PROG_START], 1, PROG_SIZE, program);
    } else {
        SDL_Log("File size %d is larger than memory space %d", file_size, MEM_SIZE);
        return 1;
    }

    if(ferror(program)) return 1;

    SDL_Log("Loaded program into memory.");
    SDL_Log("Initialised interpreter.");
    return 0;
}

/* any opcodes that require temporary variables are separated into a function */
bool execute_opcode(chip8_t *chip8)
{
    bool screen_modified = false;

    const uint16_t cur_opcode = ((chip8->mem[chip8->pc]) << 8) | (chip8->mem[chip8->pc + 1]);
    const uint8_t p           = (cur_opcode >> 12) & 0x000F; /* 0xF000 - top 4 bits */
    const uint8_t x           = (cur_opcode >> 8)  & 0x000F; /* 0x0F00 - lower 4 bits of the top byte */
    const uint8_t y           = (cur_opcode >> 4)  & 0x000F; /* 0x00F0 - upper 4 bits of the lowest byte */
    const uint8_t n           = (cur_opcode)       & 0x000F; /* 0x000F - lowest 4 bits */
    const uint8_t kk          = (cur_opcode)       & 0x00FF; /* 0x00FF - lowest 8 bits */
    const uint16_t nnn        = (cur_opcode)       & 0x0FFF; /* 0x0FFF - lowest 12 bits */
    chip8->pc += 2;

#ifdef DEBUG
    SDL_Log("0x%04X: 0x%04X", chip8->pc, cur_opcode);
#endif

    switch(p) {
        case 0x0:
            switch(kk) {
                case 0xE0: /* 00E0 - CLS */
                    memset(chip8->display, 0, sizeof(bool) * DIS_COLS * DIS_ROWS);
                    screen_modified = true;
                    break;
                case 0xEE: /* 00EE - RET */
                    chip8->pc = chip8->stack[--(chip8->sp)];
                    break;
                default:
                    SDL_LogWarn(SDL_LOG_PRIORITY_WARN, "Unknown opcode: 0x%04X", cur_opcode);
                    break;
            } break;
        case 0x1: /* 1nnn JP addr */
            chip8->pc = nnn;
            break;
        case 0x2: /* 2nnn CALL addr */
            chip8->stack[chip8->sp++] = chip8->pc;
            chip8->pc = nnn;
            break;
        case 0x3: /* 3xkk - SE Vx, byte */
            if(chip8->v[x] == kk)
                chip8->pc += 2;
            break;
        case 0x4: /* SNE Vx, byte */
            if(chip8->v[x] != kk)
                chip8->pc += 2;
            break;
        case 0x5: /* 5xy0 - SE Vx, Vy */
            if(chip8->v[x] == chip8->v[y])
                chip8->pc += 2;
            break;
        case 0x6: /* 6xkk - LD Vx, byte */
            chip8->v[x] = kk;
            break;
        case 0x7: /* 7xkk - ADD Vx, byte */
            chip8->v[x] += kk;
            break;
        case 0x8:
            switch (n) {
                case 0x0: /* 8xy0 - LD Vx, Vy */
                    chip8->v[x] = chip8->v[y];
                    break;
                case 0x1: /* 8xy1 - OR Vx, Vy */
                    chip8->v[x] |= chip8->v[y];
                    break;
                case 0x2: /* 8xy2 - AND Vx, Vy */
                    chip8->v[x] &= chip8->v[y];
                    break;
                case 0x3: /* 8xy3 - XOR Vx, Vy */
                    chip8->v[x] ^= chip8->v[y];
                    break;
                case 0x4: /* 8xy4 - ADD Vx, Vy */
                    add_and_check_carry(chip8, x, y);
                    break;
                case 0x5: /* 8xy5 - SUB Vx, Vy */
                    chip8->v[x] -= chip8->v[y];
                    if(chip8->v[x] > chip8->v[y])
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    break;
                case 0x6: /* 8xy6 - SHR Vx {, Vy} */
                    if((chip8->v[x] & 0x1) != 0)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    chip8->v[x] >>= 1;
                    break;
                case 0x7: /* 8xy7 - SUBN Vx, Vy */
                    chip8->v[x] = chip8->v[y] - chip8->v[x];
                    if(chip8->v[x] < chip8->v[y])
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    break;
                case 0xE: /* 8xyE - SHL Vx {, Vy} */
                    chip8->v[x] <<= 1;
                    if((chip8->v[x] & 0x80) != 0)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    break;
                default:
                    SDL_LogWarn(SDL_LOG_PRIORITY_WARN, "Unknown opcode: 0x%04X", cur_opcode);
                    break;
            }
            break;
        case 0x9: /* 9xy0 - SNE Vx, Vy */
            if(chip8->v[x] != chip8->v[y])
                chip8->pc += 2;
            break;
        case 0xA: /* Annn - LD I, addr */
            chip8->I = nnn;
            break;
        case 0xB: /* Bnnn - JP V0, addr */
            chip8->pc -= 2;
            chip8->pc = nnn + chip8->v[0];
            break;
        case 0xC: /* Cxkk - RND Vx, byte */
            chip8->v[x] = (rand() % 255) & kk;
            break;
        case 0xD: /* Dxyn - DRW Vx, Vy, nibble */
            draw(chip8, x, y, n);
            screen_modified = true;
            break;
        case 0xE:
            switch(kk) {
                case 0x9E: /* Ex9E - SKP Vx */
                    if(chip8->key[chip8->v[x]] == 1)
                        chip8->pc += 2;
                    break;
                case 0xA1: /* ExA1 - SKNP Vx */
                    if(chip8->key[chip8->v[x]] != 1)
                        chip8->pc += 2;
                    break;
                default:
                    SDL_LogWarn(SDL_LOG_PRIORITY_WARN, "Unknown opcode: 0x%04X", cur_opcode);
                    break;
            }
            break;
        case 0xF:
            switch(kk) {
                case 0x07: /* Fx07 - LD Vx, DT */
                    chip8->v[x] = chip8->dt;
                    break;
                case 0x0A: /* Fx0A - LD Vx, K */
                    if(get_key(chip8, x) == 0)
                       chip8->pc -= 2;
                    break;
                case 0x15: /* Fx15 - LD DT, Vx */
                    chip8-> dt = chip8->v[x];
                    break;
                case 0x18: /* Fx18 - LD ST, Vx */
                    chip8->st = chip8->v[x];
                    break;
                case 0x1E: /* Fx1E - ADD I, Vx */
                    chip8->I += chip8->v[x];
                    break;
                case 0x29: /* Fx29 - LD F, Vx */
                    chip8->I = BYTES_PER_CHAR_SPRITE * chip8->v[x];
                    break;
                case 0x33: /* Fx33 - LD B, Vx */
                    convert_decimal(chip8, x);
                    break;
                case 0x55: /* Fx55 LD [I], Vx */
                    for(uint8_t i = 0; i <= x; i++)
                        chip8->mem[chip8->I + i] = chip8->v[i];
                    break;
                case 0x65: /* Fx65 - LD Vx, [I] */
                    for(uint8_t i = 0; i <= x; i++)
                        chip8->v[i] = chip8->mem[chip8->I + i];
                    chip8->I += x + 1;
                    break;
                default:
                    SDL_LogWarn(SDL_LOG_PRIORITY_WARN, "Unknown opcode: 0x%04X", cur_opcode);
                    break;
            }
            break;
        default:
            SDL_LogWarn(SDL_LOG_PRIORITY_WARN, "Unknown opcode: 0x%04X", cur_opcode);
            break;
    }
    return screen_modified;
}

void update_timers(chip8_t *chip8)
{
    if(chip8->dt > 0)
        chip8->dt--;
    if(chip8->st > 0)
        chip8->st--;
}
