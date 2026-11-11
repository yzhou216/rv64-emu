/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* Execution of decoded instructions */

#ifndef EXEC_H
#define EXEC_H

#include <stdint.h>

#include "decode.h"
#include "rv_emu.h"

void rv_exec(struct rv_state *state, const struct rv_insn *insn);

/* Instructions by major opcode, in alu.c, mem.c, and control.c */
void emu_op(struct rv_state *state, const struct rv_insn *insn);
void emu_op_imm(struct rv_state *state, const struct rv_insn *insn);
void emu_load(struct rv_state *state, const struct rv_insn *insn);
void emu_store(struct rv_state *state, const struct rv_insn *insn);
void emu_jal(struct rv_state *state, const struct rv_insn *insn);
void emu_jalr(struct rv_state *state, const struct rv_insn *insn);
void emu_branch(struct rv_state *state, const struct rv_insn *insn);

/* Helpers of the instructions, in exec.c */
[[noreturn]] void unsupported(const char *what, uint32_t value);
void set_rd(struct rv_state *state, const struct rv_insn *insn,
	    uint64_t value);

#endif /* EXEC_H */
