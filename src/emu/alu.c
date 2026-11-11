/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "exec.h"
#include "bits.h"

/* OP instructions, keyed by funct7 and funct3 split by a digit separator */
enum rv_op : uint16_t {
	OP_ADD = 0b0000000'000,
	OP_SLL = 0b0000000'001,
	OP_SRL = 0b0000000'101,
	OP_AND = 0b0000000'111,
	OP_SUB = 0b0100000'000,
	OP_MUL = 0b0000001'000,
	OP_DIV = 0b0000001'100,
	OP_REM = 0b0000001'110,
};

/* OP-IMM instructions, keyed by funct3 */
enum rv_op_imm : uint8_t {
	OP_IMM_ADDI = 0b000,
	OP_IMM_SLLI = 0b001,
	OP_IMM_SRLI = 0b101,
};

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

	switch (funct7 << 3 | funct3) {
	case OP_ADD:
		state->regs[rd] = state->regs[rs1] + state->regs[rs2];
		break;
	case OP_SUB:
		state->regs[rd] = state->regs[rs1] - state->regs[rs2];
		break;
	case OP_SLL:
		state->regs[rd] = state->regs[rs1] <<
				  (state->regs[rs2] & 63);
		break;
	case OP_SRL:
		state->regs[rd] = state->regs[rs1] >>
				  (state->regs[rs2] & 63);
		break;
	case OP_AND:
		state->regs[rd] = state->regs[rs1] & state->regs[rs2];
		break;
	case OP_MUL:
		state->regs[rd] = state->regs[rs1] * state->regs[rs2];
		break;
	case OP_DIV:
		state->regs[rd] = emu_div(state->regs[rs1],
					  state->regs[rs2]);
		break;
	case OP_REM:
		state->regs[rd] = emu_rem(state->regs[rs1],
					  state->regs[rs2]);
		break;
	default:
		unsupported("R-type instruction", word);
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
	if (funct3 == OP_IMM_SRLI && funct6 == 0)
		state->regs[rd] = state->regs[rs1] >> shamt;
	else if (funct3 == OP_IMM_SLLI && funct6 == 0)
		state->regs[rd] = state->regs[rs1] << shamt;
	else if (funct3 == OP_IMM_ADDI)
		state->regs[rd] = state->regs[rs1] + sign_extend(imm, 12);
	else
		unsupported("I-type funct3", funct3);
	state->pc += 4; /* Next instruction */

	state->analysis.alu++;
}
