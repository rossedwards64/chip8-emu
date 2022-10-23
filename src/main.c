#include <stdint.h>
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

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
        printf("Couldn't open file.\n");
    else
        printf("Opened file %s.\n", filename);

    chip8_t chip8;

    if(init_sdl() > 0) return EXIT_FAILURE;
    if(init_emu(file, &chip8) > 0) return EXIT_FAILURE;
    fclose(file);

    while(!quit) {
        #ifdef DEBUG
        print_struct(&chip8);
        #endif
        if(execute_opcode(&chip8) == SCREEN_MODIFIED)
            render(chip8.display);
        handle_inputs(&quit, chip8.key);
        usleep(SDL_REFRESH);
    }

    close_sdl();
    return 0;
}
