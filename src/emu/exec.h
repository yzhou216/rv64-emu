/* Execution of instructions */

#ifndef EXEC_H
#define EXEC_H

#include <stdint.h>

#include "rv_emu.h"

void rv_exec(struct rv_state *state, uint32_t word);

/* Instructions by major opcode, in alu.c, mem.c, and control.c */
void emu_r_type(struct rv_state *state, uint32_t word);
void emu_i_type(struct rv_state *state, uint32_t word);
void emu_load(struct rv_state *state, uint32_t word);
void emu_s_type(struct rv_state *state, uint32_t word);
void emu_ucond_j(struct rv_state *state, uint32_t word);
void emu_jalr(struct rv_state *state, uint32_t word);
void emu_b_type(struct rv_state *state, uint32_t word);

/* Helpers of the instructions, in exec.c */
void unsupported(char *what, uint32_t value);

#endif /* EXEC_H */
