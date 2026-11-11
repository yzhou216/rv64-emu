/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* RISC-V emulator types and constants */

#ifndef RV_EMU_H
#define RV_EMU_H

#include <stddef.h>
#include <stdint.h>

#include "analysis.h"

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

/* Cache types */
enum cache_type {
	CACHE_NONE = 0,
	CACHE_DM,
	CACHE_SA
};

constexpr int CACHE_MAX_SLOTS = 4096;
constexpr int CACHE_MAX_BLOCK_SIZE = 4;

struct cache_slot {
	bool valid;
	uint64_t tag;
	uint32_t block[CACHE_MAX_BLOCK_SIZE];
	uint64_t timestamp; /* Only used by the set-associative cache */
};

struct cache {
	struct cache_slot slots[CACHE_MAX_SLOTS];
	enum cache_type type;
	int size;
	int ways;
	int block_size;
	uint64_t block_mask;
	uint64_t block_bits;
	uint64_t index_mask;
	uint64_t index_bits;
	int refs;
	int hits;
	int misses;
	int misses_cold;
	int misses_hot;
};

struct rv_state {
	uint64_t regs[RV_NUM_REGS];
	uint64_t pc;

	alignas(16) uint8_t stack[STACK_SIZE];

	bool analyze;
	bool simulate_cache;

	struct rv_analysis analysis;
	struct cache icache;
};


void rv_init(struct rv_state *state, uint32_t *target,
	     uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3);
uint64_t rv_emulate(struct rv_state *state);

[[nodiscard]] bool cache_valid(const struct cache *cache);
void cache_init(struct cache *cache);
[[nodiscard]] uint32_t cache_lookup(struct cache *cache, uint64_t addr);
void cache_print(const struct cache *cache, const char *name);

/* Logging for -v, enabled by verbose_on */
extern bool verbose_on;
[[gnu::format(printf, 1, 2)]] void verbose(const char *fmt, ...);

#endif /* RV_EMU_H */
