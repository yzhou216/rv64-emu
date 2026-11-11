/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>

#include "exec.h"
#include "bits.h"

/*
 * Major opcodes.  The digit separators split each opcode into inst[6:5]
 * and inst[4:2], the row and column of the opcode map in the RISC-V
 * specification, and inst[1:0], which is 0b11 for 32-bit instructions.
 */
enum rv_opcode : uint8_t {
	OPCODE_LOAD = 0b00'000'11,
	OPCODE_OP_IMM = 0b00'100'11,
	OPCODE_STORE = 0b01'000'11,
	OPCODE_OP = 0b01'100'11,
	OPCODE_BRANCH = 0b11'000'11,
	OPCODE_JALR = 0b11'001'11,
	OPCODE_JAL = 0b11'011'11,
};

[[noreturn]] void unsupported(char *what, uint32_t value)
{
	printf("unsupported %s 0x%x\n", what, value);
	exit(-1);
}

/* Execute word, leaving pc at the instruction to run next */
void rv_exec(struct rv_state *state, uint32_t word)
{
	uint32_t opcode = get_bits(word, 0, 7);

	switch (opcode) {
	case OPCODE_OP:
		emu_r_type(state, word);
		break;
	case OPCODE_OP_IMM:
		emu_i_type(state, word);
		break;
	case OPCODE_LOAD:
		emu_load(state, word);
		break;
	case OPCODE_STORE:
		emu_s_type(state, word);
		break;
	case OPCODE_JAL:
		emu_ucond_j(state, word);
		break;
	case OPCODE_JALR:
		emu_jalr(state, word);
		break;
	case OPCODE_BRANCH:
		emu_b_type(state, word);
		break;
	default:
		unsupported("Unknown opcode", opcode);
	}

	/* x0 is hardwired to zero, so discard any write to it */
	state->regs[RV_ZERO] = 0;
}
