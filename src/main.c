
#include "interpreter.h"


int main(int argc, char **argv)
{
    FILE *file = fopen(argv[0], "rb");
    uint16_t opcode;

    init(file);
    while(1) {
        /* FETCH */

        /* DECODE */
        parse_opcode(opcode);

        /* EXECUTE */
    }

    fclose(file);
}
