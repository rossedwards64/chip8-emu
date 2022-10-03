#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdio.h>
#include <stdint.h>

void init(FILE *buffer);
void fetch_opcode();
void parse_opcode();

/* OPCODE FUNCTIONS */
void cls();
void draw(uint8_t v_x, uint8_t v_y, uint8_t addr);
void update_delay(uint8_t v_x);
void update_sound(uint8_t v_x);
void dump_register(uint8_t v_x, uint8_t *instr);
void load_register(uint8_t v_x, uint8_t *instr);

#endif // INTERPRETER_H_
