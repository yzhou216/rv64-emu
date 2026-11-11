/*
 * SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "decode.h"
#include "bits.h"

struct rv_insn rv_decode(uint32_t word)
{
	return (struct rv_insn){
		.word = word,
		.opcode = word,
		.rd = word >> 7,
		.funct3 = word >> 12,
		.rs1 = word >> 15,
		.rs2 = word >> 20,
		.funct7 = word >> 25,
	};
}

/*
 * Sign-extended immediates.  Converting an N-bit value to signed
 * _BitInt(N), which GCC reduces modulo 2^N, sign-extends it from bit
 * N - 1.
 */
int64_t imm_i(const struct rv_insn *insn)
{
	return (signed _BitInt(12))(insn->word >> 20);
}

int64_t imm_s(const struct rv_insn *insn)
{
	uint32_t imm = get_bits(insn->word, 7, 5) |
		       get_bits(insn->word, 25, 7) << 5;

	return (signed _BitInt(12))imm;
}

int64_t imm_b(const struct rv_insn *insn)
{
	uint32_t imm = get_bits(insn->word, 8, 4) << 1 |
		       get_bits(insn->word, 25, 6) << 5 |
		       get_bit(insn->word, 7) << 11 |
		       get_bit(insn->word, 31) << 12;

	return (signed _BitInt(13))imm;
}

int64_t imm_j(const struct rv_insn *insn)
{
	uint32_t imm = get_bits(insn->word, 21, 10) << 1 |
		       get_bit(insn->word, 20) << 11 |
		       get_bits(insn->word, 12, 8) << 12 |
		       get_bit(insn->word, 31) << 20;

	return (signed _BitInt(21))imm;
}
