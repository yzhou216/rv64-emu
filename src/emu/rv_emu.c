/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdcountof.h>
#include <stdio.h>

#include "rv_emu.h"
#include "decode.h"
#include "exec.h"

/* Fetch, decode, and execute the instruction at pc */
static void rv_one(struct rv_state *state)
{
	/* cache_lookup() reads the word from memory unless it is cached */
	auto insn = rv_decode(cache_lookup(&state->icache, state->pc));

	rv_exec(state, &insn);
}

void rv_init(struct rv_state *state, uint32_t *target,
	     uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3)
{
	state->pc = (uint64_t)target;
	state->regs[RV_A0] = a0;
	state->regs[RV_A1] = a1;
	state->regs[RV_A2] = a2;
	state->regs[RV_A3] = a3;

	state->regs[RV_ZERO] = 0; /* constant zero register */
	state->regs[RV_RA] = RV_STOP;
	state->regs[RV_SP] = (uint64_t)&state->stack[countof(state->stack)];

	state->analysis = (struct rv_analysis){};
	cache_init(&state->icache);
}

uint64_t rv_emulate(struct rv_state *state)
{
	while (state->pc != RV_STOP) {
		rv_one(state);
		state->analysis.insns++;
	}
	return state->regs[RV_A0];
}
