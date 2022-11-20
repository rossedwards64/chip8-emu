#ifndef UTIL_H_
#define UTIL_H_

#define DEBUG

#define MEM_SIZE 0x1000
#define PROG_START 0x200
#define PROG_SIZE (MEM_SIZE - PROG_START)
#define ARR_SIZE 16
#define BYTES_PER_CHAR_SPRITE 5

#define CHIP8_REFRESH 60
#define SDL_REFRESH CHIP8_REFRESH * 100

#define WIN_COLS 640
#define WIN_ROWS 320
#define DIS_COLS 64
#define DIS_ROWS 32

#define BLACK 0x0
#define WHITE 0xFF

#endif // UTIL_H_
