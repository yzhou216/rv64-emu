/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "exec.h"
#include "bits.h"

void emu_ucond_j(struct rv_state *state, uint32_t word)
{
	uint32_t rd = get_bits(word, 7, 5);
	if (rd != 0) /* jal, link ra, else j, ra remain unchanged  */
		state->regs[rd] = (uint64_t)state->pc + 4;

	uint32_t imm20 = get_bit(word, 31);
	uint32_t imm10_1 = get_bits(word, 21, 10);
	uint32_t imm11 = get_bit(word, 20);
	uint32_t imm19_12 = get_bits(word, 12, 8);
	uint32_t imm = (imm20 << 20) |
		       (imm10_1 << 1) |
		       (imm11 << 11) |
		       (imm19_12 << 12);
	int32_t offset = sign_extend(imm, 21);

	state->pc += offset;

	state->analysis.jumps++;
}

void emu_jalr(struct rv_state *state, uint32_t word)
{
	uint32_t rd = get_bits(word, 7, 5);
	uint32_t rs1 = get_bits(word, 15, 5);
	uint32_t imm = get_bits(word, 20, 12);
	uint64_t target = (state->regs[rs1] + sign_extend(imm, 12)) &
			  ~(uint64_t)1;

	/* Compute the target before writing rd, which may be rs1 */
	state->regs[rd] = state->pc + 4;
	state->pc = target;

	state->analysis.jumps++;
}

void emu_b_type(struct rv_state *state, uint32_t word)
{
	uint32_t rs1 = get_bits(word, 15, 5);
	uint32_t rs2 = get_bits(word, 20, 5);
	uint32_t funct3 = get_bits(word, 12, 3);
	uint32_t imm4_1 = get_bits(word, 8, 4);
	uint32_t imm11 = get_bits(word, 7, 1);
	uint32_t imm12 = get_bits(word, 31, 1);
	uint32_t imm10_5 = get_bits(word, 25, 6);
	uint32_t imm = (imm4_1 << 1) |
		       (imm11 << 11) |
		       (imm12 << 12) |
		       (imm10_5 << 5);

	/* Register values for comparison and the label offset are signed */
	int64_t rs1_val = (int64_t)state->regs[rs1];
	int64_t rs2_val = (int64_t)state->regs[rs2];
	int32_t offset = sign_extend(imm, 13);

	bool taken = false;
	switch (funct3) {
	case 0b000: /* BEQ */
		taken = rs1_val == rs2_val;
		break;
	case 0b001: /* BNE */
		taken = rs1_val != rs2_val;
		break;
	case 0b100: /* BLT */
		taken = rs1_val < rs2_val;
		break;
	case 0b101: /* BGE */
		taken = rs1_val >= rs2_val;
		break;
	default:
		unsupported("B-type funct3", funct3);
	}

	if (taken) {
		state->pc += offset;
		state->analysis.taken++;
	} else {
		state->pc += 4; /* Next instruction */
		state->analysis.not_taken++;
	}
}
