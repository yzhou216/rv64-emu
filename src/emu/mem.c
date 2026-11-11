/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "exec.h"

/* Access widths of LOAD and STORE instructions, encoded in funct3 */
enum rv_width : uint8_t {
	WIDTH_BYTE = 0b000,
	WIDTH_WORD = 0b010,
	WIDTH_DOUBLE = 0b011,
};

void emu_load(struct rv_state *state, const struct rv_insn *insn)
{
	uint8_t *addr = (uint8_t *)state->regs[insn->rs1] + imm_i(insn);
	uint64_t result;

	switch (insn->funct3) {
	case WIDTH_BYTE: /* lb */
		result = *(int8_t *)addr;
		break;
	case WIDTH_WORD: /* lw */
		result = *(int32_t *)addr;
		break;
	case WIDTH_DOUBLE: /* ld */
		result = *(uint64_t *)addr;
		break;
	default:
		unsupported(insn);
	}

	set_rd(state, insn, result);
	state->pc += 4; /* Next instruction */
	state->analysis.loads++;
}

void emu_store(struct rv_state *state, const struct rv_insn *insn)
{
	uint8_t *addr = (uint8_t *)state->regs[insn->rs1] + imm_s(insn);
	uint64_t rs2_val = state->regs[insn->rs2];

	switch (insn->funct3) {
	case WIDTH_BYTE: /* sb */
		*addr = rs2_val;
		break;
	case WIDTH_WORD: /* sw */
		*(uint32_t *)addr = rs2_val;
		break;
	case WIDTH_DOUBLE: /* sd */
		*(uint64_t *)addr = rs2_val;
		break;
	default:
		unsupported(insn);
	}

	state->pc += 4;
	state->analysis.stores++;
}
