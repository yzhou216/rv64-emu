/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "exec.h"
#include "bits.h"

/*
 * Signed division and remainder as RISC-V defines them.  Unlike C, it
 * defines the results of dividing by zero and of dividing the most
 * negative value by -1, which overflows.
 */
static uint64_t emu_div(int64_t dividend, int64_t divisor)
{
	if (divisor == 0)
		return UINT64_MAX;
	if (dividend == INT64_MIN && divisor == -1)
		return dividend;
	return dividend / divisor;
}

static uint64_t emu_rem(int64_t dividend, int64_t divisor)
{
	if (divisor == 0)
		return dividend;
	if (dividend == INT64_MIN && divisor == -1)
		return 0;
	return dividend % divisor;
}

void emu_r_type(struct rv_state *state, uint32_t word)
{
	uint32_t rd = get_bits(word, 7, 5);
	uint32_t rs1 = get_bits(word, 15, 5);
	uint32_t rs2 = get_bits(word, 20, 5);
	uint32_t funct3 = get_bits(word, 12, 3);
	uint32_t funct7 = get_bits(word, 25, 7);

	switch (funct3) {
	case 0b000:
		if (funct7 == 0b0000000) /* ADD */
			state->regs[rd] = state->regs[rs1] + state->regs[rs2];
		else if (funct7 == 0b0100000) /* SUB */
			state->regs[rd] = state->regs[rs1] - state->regs[rs2];
		else if (funct7 == 0b0000001) /* MUL */
			state->regs[rd] = state->regs[rs1] * state->regs[rs2];
		else
			unsupported("R-type funct7", funct7);
		break;
	case 0b001:
		if (funct7 == 0b0000000) /* SLL */
			state->regs[rd] = state->regs[rs1] <<
				  (state->regs[rs2] & 63);
		else
			unsupported("R-type funct7", funct7);
		break;
	case 0b100:
		if (funct7 == 0b0000001) /* DIV */
			state->regs[rd] = emu_div(state->regs[rs1],
					  state->regs[rs2]);
		else
			unsupported("R-type funct7", funct7);
		break;
	case 0b101:
		if (funct7 == 0b0000000) /* SRL */
			state->regs[rd] = state->regs[rs1] >>
				  (state->regs[rs2] & 63);
		else
			unsupported("R-type funct7", funct7);
		break;
	case 0b110:
		if (funct7 == 0b0000001) /* REM */
			state->regs[rd] = emu_rem(state->regs[rs1],
					  state->regs[rs2]);
		else
			unsupported("R-type funct7", funct7);
		break;
	case 0b111:
		if (funct7 == 0b0000000) /* AND */
			state->regs[rd] = state->regs[rs1] & state->regs[rs2];
		else
			unsupported("R-type funct7", funct7);
		break;
	default:
		unsupported("R-type funct3", funct3);
	}

	state->pc += 4; /* Next instruction */
	state->analysis.alu++;
}

void emu_i_type(struct rv_state *state, uint32_t word)
{
	uint32_t rd = get_bits(word, 7, 5);
	uint32_t rs1 = get_bits(word, 15, 5);
	/* RV64 shift amounts have six bits, leaving six bits of funct6 */
	uint32_t shamt = get_bits(word, 20, 6);
	uint32_t funct3 = get_bits(word, 12, 3);
	uint32_t funct6 = get_bits(word, 26, 6);
	uint32_t imm = get_bits(word, 20, 12);
	if (funct3 == 0b101 && funct6 == 0b0)
		state->regs[rd] = state->regs[rs1] >> shamt;
	else if (funct3 == 0b001 && funct6 == 0b0) /* SLLI */
		state->regs[rd] = state->regs[rs1] << shamt;
	else if (funct3 == 0b000)
		state->regs[rd] = state->regs[rs1] + sign_extend(imm, 12);
	else
		unsupported("I-type funct3", funct3);
	state->pc += 4; /* Next instruction */

	state->analysis.alu++;
}
