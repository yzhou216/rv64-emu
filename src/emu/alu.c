/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "exec.h"

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

void emu_op(struct rv_state *state, const struct rv_insn *insn)
{
	uint64_t rs1_val = state->regs[insn->rs1];
	uint64_t rs2_val = state->regs[insn->rs2];
	uint64_t result;

	/*
	 * Bit-precise types are not promoted, so widen funct7 before the
	 * shift instead of losing its high bits.
	 */
	switch ((unsigned int)insn->funct7 << 3 | insn->funct3) {
	case OP_ADD:
		result = rs1_val + rs2_val;
		break;
	case OP_SUB:
		result = rs1_val - rs2_val;
		break;
	case OP_SLL:
		result = rs1_val << (rs2_val & 63);
		break;
	case OP_SRL:
		result = rs1_val >> (rs2_val & 63);
		break;
	case OP_AND:
		result = rs1_val & rs2_val;
		break;
	case OP_MUL:
		result = rs1_val * rs2_val;
		break;
	case OP_DIV:
		result = emu_div(rs1_val, rs2_val);
		break;
	case OP_REM:
		result = emu_rem(rs1_val, rs2_val);
		break;
	default:
		unsupported(insn);
	}

	set_rd(state, insn, result);
	state->pc += 4; /* Next instruction */
	state->analysis.alu++;
}

void emu_op_imm(struct rv_state *state, const struct rv_insn *insn)
{
	uint64_t rs1_val = state->regs[insn->rs1];
	/* RV64 shifts take six bits of shamt, leaving six bits of funct6 */
	unsigned _BitInt(6) shamt = insn->word >> 20;
	unsigned _BitInt(6) funct6 = insn->word >> 26;
	uint64_t result;

	if (insn->funct3 == OP_IMM_SRLI && funct6 == 0)
		result = rs1_val >> shamt;
	else if (insn->funct3 == OP_IMM_SLLI && funct6 == 0)
		result = rs1_val << shamt;
	else if (insn->funct3 == OP_IMM_ADDI)
		result = rs1_val + imm_i(insn);
	else
		unsupported(insn);

	set_rd(state, insn, result);
	state->pc += 4; /* Next instruction */
	state->analysis.alu++;
}
