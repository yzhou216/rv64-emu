/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "exec.h"

/* Conditions of BRANCH instructions, encoded in funct3 */
enum rv_branch : uint8_t {
	BRANCH_EQ = 0b000,
	BRANCH_NE = 0b001,
	BRANCH_LT = 0b100,
	BRANCH_GE = 0b101,
};

void emu_jal(struct rv_state *state, const struct rv_insn *insn)
{
	set_rd(state, insn, state->pc + 4);

	state->pc += imm_j(insn);
	state->analysis.jumps++;
}

void emu_jalr(struct rv_state *state, const struct rv_insn *insn)
{
	uint64_t link = state->pc + 4;

	/* Read rs1 before writing rd, which may be the same register */
	state->pc = (state->regs[insn->rs1] + imm_i(insn)) & ~(uint64_t)1;
	set_rd(state, insn, link);
	state->analysis.jumps++;
}

void emu_branch(struct rv_state *state, const struct rv_insn *insn)
{
	/* Register values for comparison and the label offset are signed */
	int64_t rs1_val = (int64_t)state->regs[insn->rs1];
	int64_t rs2_val = (int64_t)state->regs[insn->rs2];
	bool taken = false;

	switch (insn->funct3) {
	case BRANCH_EQ:
		taken = rs1_val == rs2_val;
		break;
	case BRANCH_NE:
		taken = rs1_val != rs2_val;
		break;
	case BRANCH_LT:
		taken = rs1_val < rs2_val;
		break;
	case BRANCH_GE:
		taken = rs1_val >= rs2_val;
		break;
	default:
		unsupported(insn);
	}

	if (taken) {
		state->pc += imm_b(insn);
		state->analysis.taken++;
	} else {
		state->pc += 4; /* Next instruction */
		state->analysis.not_taken++;
	}
}
