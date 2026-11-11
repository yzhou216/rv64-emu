/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* RISC-V emulator state and interface */

#ifndef RV_EMU_H
#define RV_EMU_H

#include <stddef.h>
#include <stdint.h>

#include "analysis.h"
#include "cache/cache.h"

/* ABI names of the registers that the emulator sets up */
enum rv_reg : uint8_t {
	RV_ZERO = 0,
	RV_RA = 1,
	RV_SP = 2,
	RV_A0 = 10,
	RV_A1 = 11,
	RV_A2 = 12,
	RV_A3 = 13,
};

constexpr int RV_NUM_REGS = 32;

/* The calling convention keeps sp 16-byte aligned */
constexpr size_t STACK_SIZE = 8192;
static_assert(STACK_SIZE % 16 == 0);

/* Sentinel value for stopping the emulator */
constexpr uint64_t RV_STOP = 0;

struct rv_state {
	uint64_t regs[RV_NUM_REGS];
	uint64_t pc;

	alignas(16) uint8_t stack[STACK_SIZE];

	struct rv_analysis analysis;
	struct cache icache;
};

void rv_init(struct rv_state *state, const uint32_t *target,
	     uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3);
uint64_t rv_emulate(struct rv_state *state);

#endif /* RV_EMU_H */
