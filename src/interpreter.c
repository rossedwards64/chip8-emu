#include "interpreter.h"


static uint8_t fontset[80] = { 0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
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

uint8_t init_emu(FILE *buffer, chip8_t *chip8)
{
    srand(0);
    memset(chip8->display, 0, sizeof(bool) * DIS_COLS * DIS_ROWS);
    memset(chip8->mem, 0, sizeof(chip8->mem) / sizeof(chip8->mem[0]));
    memset(chip8->v, 0, sizeof(chip8->v) / sizeof(chip8->v[0]));
    memset(chip8->stack, 0, sizeof(chip8->stack) / sizeof(chip8->stack[0]));
    memset(chip8->key, 0, sizeof(chip8->key) / sizeof(chip8->key[0]));
    chip8->I = 0;
    chip8->sp = 0;
    chip8->dt = 0;
    chip8->st = 0;
    chip8->pc = PROG_START;

    for(uint8_t i = 0x0, j = 0x50; j <= 0x9F; ++i, ++j)
        chip8->mem[j] = fontset[i];
    SDL_Log("Loaded font into memory.");

    fseek(buffer, 0, SEEK_END);
    uint16_t file_size = ftell(buffer);
    rewind(buffer);

    if(file_size < PROG_SIZE) {
        fread(&chip8->mem[PROG_START], 1, PROG_SIZE, buffer);
    } else {
        SDL_Log("File size %d is larger than memory space %d", file_size, MEM_SIZE);
        return 1;
    }

    if(ferror(buffer)) return 1;

    SDL_Log("Loaded program into memory.");
    SDL_Log("Initialised interpreter.");
    return 0;
}

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
    SDL_Log("0x%04X: 0x%04X", chip8->pc, cur_opcode);

    switch(p) {
        case 0x0:
            switch(y) {
                case 0x0:
                    switch(kk) {
                        case 0xE0:
                            memset(chip8->display, 0, sizeof(bool) * DIS_COLS * DIS_ROWS);
                            screen_modified = true;
                            break;
                        case 0xEE:
                            chip8->pc = chip8->stack[--(chip8->sp)];
                            break;
                        default:
                            SDL_Log("Unknown opcode: 0x%04X", cur_opcode);
                            break;
                    } break;
            } break;
        case 0x1:
            chip8->pc = nnn;
            break;
        case 0x2:
            chip8->stack[chip8->sp++] = chip8->pc;
            chip8->pc = nnn;
            break;
        case 0x3:
            if(chip8->v[x] == kk)
                chip8->pc += 2;
            break;
        case 0x4:
            if(chip8->v[x] != kk)
                chip8->pc += 2;
            break;
        case 0x5:
            if(chip8->v[x] == chip8->v[y])
                chip8->pc += 2;
            break;
        case 0x6:
            chip8->v[x] = kk;
            break;
        case 0x7:
            chip8->v[x] += kk;
            break;
        case 0x8:
            switch (n) {
                case 0x0:
                    chip8->v[x] = chip8->v[y];
                    break;
                case 0x1:
                    chip8->v[x] |= chip8->v[y];
                    break;
                case 0x2:
                    chip8->v[x] &= chip8->v[y];
                    break;
                case 0x3:
                    chip8->v[x] ^= chip8->v[y];
                    break;
                case 0x4:
                    if((chip8->v[x] + chip8->v[y]) > 255)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    chip8->v[x] += chip8->v[y];
                    break;
                case 0x5:
                    if(chip8->v[x] > chip8->v[y])
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    chip8->v[x] -= chip8->v[y];
                    break;
                case 0x6:
                    chip8->v[x] >>= 1;
                    if((chip8->v[x] & -(chip8->v[x])) == 1)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    break;
                case 0x7:
                    if(chip8->v[x] < chip8->v[y])
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 1;
                    chip8->v[x] -= chip8->v[y];
                    break;
                case 0xE:
                    chip8->v[x] = chip8->v[x] << 1;
                    if((chip8->v[x] & -(chip8->v[x])) == 1)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    break;
                default:
                    SDL_Log("Unknown opcode: 0x%04X", cur_opcode);
                    break;
            }
            break;
        case 0x9:
            if(chip8->v[x] != chip8->v[y])
                chip8->pc += 2;
            break;
        case 0xA:
            chip8->I = nnn;
            break;
        case 0xB:
            chip8->pc -= 2;
            chip8->pc = nnn + chip8->v[0];
            break;
        case 0xC:
            chip8->v[x] = (rand() % 255) & kk;
            break;
        case 0xD:
            draw(chip8->display, chip8->mem, chip8->I, chip8->v[x], chip8->v[y], n, &(chip8->v[0xF]));
            screen_modified = true;
            break;
        case 0xE:
            switch(kk) {
                case 0x9E:
                    if(chip8->key[chip8->v[x]] == 1)
                        chip8->pc += 2;
                    break;
                case 0xA1:
                    if(chip8->key[chip8->v[x]] != 1)
                        chip8->pc += 2;
                    break;
                default:
                    SDL_Log("Unknown opcode: 0x%04X", cur_opcode);
                    break;
            }
            break;
        case 0xF:
            switch(kk) {
                case 0x07:
                    chip8->v[x] = chip8->dt;
                    break;
                case 0x0A:
                    if(get_key(&(chip8->v[x]), chip8->key) == 0)
                       chip8->pc -= 2;
                    break;
                case 0x15:
                    chip8-> dt = chip8->v[x];
                    break;
                case 0x18:
                    chip8->st = chip8->v[x];
                    break;
                case 0x1E:
                    chip8->I += chip8->v[x];
                    break;
                case 0x29:
                    chip8->I = BYTES_PER_CHAR_SPRITE * chip8->v[x];
                    break;
                case 0x33:
                    convert_decimal(chip8->mem, chip8->I, &(chip8->v[x]));
                    break;
                case 0x55:
                    for(uint8_t i = 0; i <= x; i++)
                        chip8->mem[chip8->I + i] = chip8->v[i];
                    break;
                case 0x65:
                    for(uint8_t i = 0; i <= x; i++)
                        chip8->v[i] = chip8->mem[chip8->I + i];
                    chip8->I += x + 1;
                    break;
                default:
                    SDL_Log("Unknown opcode: 0x%04X", cur_opcode);
                    break;
            }
            break;
        default:
            SDL_Log("Unknown opcode: 0x%04X", cur_opcode);
            break;
    }
    return screen_modified;
}

/* OPCODE FUNCTIONS */
bool get_key(uint8_t *v_x, bool key[ARR_SIZE])
{
    bool key_pressed = false;
    for (uint8_t i = 0; i < ARR_SIZE && !key_pressed; i++) {
        if(key[i] == true) {
            key_pressed = true;
            *v_x = key[i];
        }
    }
    return key_pressed;
}

void update_timers(chip8_t *chip8)
{
    if(chip8->dt > 0)
        chip8->dt--;
    if(chip8->st > 0)
        chip8->st--;
}

void draw(bool display[DIS_ROWS][DIS_COLS], uint8_t mem[MEM_SIZE],
          uint16_t I, uint8_t v_x, uint8_t v_y, uint16_t n, uint8_t *coll_flag)
{
    uint16_t pixel = 0;
    uint8_t y = 0, x = 0;

    for(uint16_t i = 0; i < n; ++i) {
        if(y >= DIS_ROWS) break;
        pixel = mem[I + i];
        y = (v_y + i) % DIS_ROWS;
        for(uint8_t j = 0; j < 8; ++j) {
            if(x >= DIS_COLS) break;
            x = (v_x + j) % DIS_COLS;
            if((pixel & (0x80 >> j)) != 0) {
                if(display[y][x] == 1)
                    *coll_flag = 1;
                display[y][x] ^= 1;
            }
        }
    }
}

void convert_decimal(uint8_t mem[MEM_SIZE], uint16_t I, uint8_t *v_x)
{
    mem[I] = *v_x / 100;
    mem[I + 1] = (*v_x % 100) / 10;
    mem[I + 2] = *v_x % 10;
}
