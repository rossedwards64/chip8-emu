#include <stdio.h>

#include "interpreter.h"
#include "gfx.h"


bool quit = false;

int main(int argc, char **argv)
{
    FILE *file = fopen("programs/IBM Logo.ch8", "rb");
    if (file == NULL) {
        printf("coulnt open file\n");
    }

    init_sdl();

    init(file);
    while(!quit) {
        render();
        parse_opcode();
        handle_inputs(&quit);
        update();
    }

    fclose(file);
    close_sdl();
    return 0;
}
