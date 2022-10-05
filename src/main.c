
#include "interpreter.h"
#include <stdio.h>


int main(int argc, char **argv)
{
    FILE *file = fopen("programs/IBM Logo.ch8", "rb");
    if (file == NULL) {
        printf("coulnt open file\n");
    }
    uint16_t opcode;

    init(file);
    for(int i = 0; i < 20; i++) {
        /* FETCH */

        /* DECODE */
        parse_opcode(opcode);

        /* EXECUTE */
    }

    fclose(file);
    return 0;
}
