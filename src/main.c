#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "interpreter.h"
#include "gfx.h"

#undef DEBUG
#define SCREEN_MODIFIED 1

bool quit = false;

int main(int argc, char **argv)
{
    char *filename;
    if(argv[1] == NULL)
        filename = "programs/IBM Logo.ch8";
    else
        filename = argv[1];

    FILE *program = fopen(filename, "rb");

    if (program == NULL)
        printf("Couldn't open file.\n");
    else
        printf("Opened file %s.\n", filename);

    if(init_sdl() > 0) {
        printf("Could not initialise SDL.\n");
        return EXIT_FAILURE;
    }

    chip8_t chip8;

    if(init_emu(&chip8, program) > 0) {
        printf("Could not initialise the interpreter.\n");
        return EXIT_FAILURE;
    }

    fclose(program);

    while(!quit) {
#ifdef DEBUG
        print_reg(&chip8);
#endif
        if(execute_opcode(&chip8) == SCREEN_MODIFIED)
            render(chip8.display);
        handle_inputs(&quit, chip8.key);
        update_timers(&chip8);
        usleep(SDL_REFRESH);
    }

    close_sdl();
    return 0;
}
