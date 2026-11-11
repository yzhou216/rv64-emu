/*
 * SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* RISC-V instruction formats */

#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

/*
 * Fields at the same bit positions in every instruction format.  Each
 * field is an unsigned _BitInt as wide as the field, and converting the
 * instruction word to such a type keeps its low bits, so decoding a
 * field takes only a shift.
 */
struct rv_insn {
	uint32_t word;
	unsigned _BitInt(7) opcode;
	unsigned _BitInt(5) rd;
	unsigned _BitInt(3) funct3;
	unsigned _BitInt(5) rs1;
	unsigned _BitInt(5) rs2;
	unsigned _BitInt(7) funct7;
};

[[nodiscard]] struct rv_insn rv_decode(uint32_t word);

/* Sign-extended immediates of the I, S, B, and J formats */
[[nodiscard]] int64_t imm_i(const struct rv_insn *insn);
[[nodiscard]] int64_t imm_s(const struct rv_insn *insn);
[[nodiscard]] int64_t imm_b(const struct rv_insn *insn);
[[nodiscard]] int64_t imm_j(const struct rv_insn *insn);

#endif /* DECODE_H */
