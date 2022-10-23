#ifndef UTIL_H_
#define UTIL_H_

#define DEBUG

#define MEM_SIZE 0x1000
#define PROG_START 0x200
#define PROG_SIZE (0x1000 - 0x200)
#define ARR_SIZE 16

#define CHIP8_REFRESH 60
#define SDL_REFRESH CHIP8_REFRESH * 1000

#define WIN_ROWS 1280
#define WIN_COLS 720
#define DIS_ROWS 64
#define DIS_COLS 32

#define BLACK 0
#define WHITE 0xFF

#endif // UTIL_H_
