/* Execution of instructions */

#ifndef EXEC_H
#define EXEC_H

#include <stdint.h>

#include "rv_emu.h"

void rv_exec(struct rv_state *state, uint32_t word);

#endif /* EXEC_H */
