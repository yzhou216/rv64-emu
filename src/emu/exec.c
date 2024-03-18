#include <stdio.h>
#include <stdlib.h>

#include "exec.h"
#include "bits.h"

void unsupported(char *what, uint32_t value)
{
	printf("unsupported %s 0x%x\n", what, value);
	exit(-1);
}

/* Execute word, leaving pc at the instruction to run next */
void rv_exec(struct rv_state *state, uint32_t word)
{
	uint32_t opcode = get_bits(word, 0, 7);

	switch (opcode) {
	case 0b0110011:
		emu_r_type(state, word);
		break;
	case 0b0010011:
		emu_i_type(state, word);
		break;
	case 0b0100011:
		emu_s_type(state, word);
		break;
	case 0b1100111:
		/* JALR (RET) is a variant of I-type instructions */
		emu_jalr(state, word);
		break;
	case 0b1100011:
		emu_b_type(state, word);
		break;
	default:
		unsupported("Unknown opcode", opcode);
	}

	/* x0 is hardwired to zero, so discard any write to it */
	state->regs[RV_ZERO] = 0;
}
