#include "interpreter.h"
#include "gfx.h"


bool quit = false;

int main(int argc, char **argv)
{
    FILE *file = fopen("programs/IBM Logo.ch8", "rb");
    if (file == NULL)
        printf("Couldn't open file.\n");
    else
        printf("Opened file.\n");

    chip8_t chip8;

    if(init_sdl() > 0)
        return EXIT_FAILURE;

    if(init_emu(file, &chip8) > 0)
        return EXIT_FAILURE;

    while(!quit) {
        #ifdef DEBUG
        print_op(&chip8);
        #endif
        parse_opcode(&chip8);
        render(chip8.display);
        handle_inputs(&quit);
    }

    fclose(file);
    close_sdl();
    return 0;
}
