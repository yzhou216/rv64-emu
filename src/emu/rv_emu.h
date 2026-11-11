/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* RISC-V emulator types and constants */

#include <stdint.h>
#include <stdbool.h>

#include "analysis.h"

/* ABI register mappings */
#define RV_ZERO      0
#define RV_RA        1
#define RV_SP        2
#define RV_A0       10
#define RV_A1       11
#define RV_A2       12
#define RV_A3       13
#define RV_NUM_REGS 32

/* Opcodes for instr formats */
enum rv_format {
	FMT_R = 0b0110011,
	FMT_I_LOAD = 0b0000011,
	FMT_I_JALR = 0b1100111,
	FMT_I_ARITH = 0b0010011,
	FMT_S = 0b0100011,
	FMT_SB = 0b1100011,
	FMT_UJ = 0b1101111
};

/* Width values for FMT_I_LOAD and FMT_S */
#define LDST_BYTE   0
#define LDST_WORD   2
#define LDST_DOUBLE 3

#define STACK_SIZE 8192

/* Sentinel value for stopping the emulator */
#define RV_STOP 0

/* Cache types */
enum cache_type {
	CACHE_NONE = 0,
	CACHE_DM,
	CACHE_SA
};

#define CACHE_MAX_SLOTS 4096
#define CACHE_MAX_BLOCK_SIZE 4

struct cache_slot {
	uint64_t valid;
	uint64_t tag;
	uint32_t block[CACHE_MAX_BLOCK_SIZE];
	uint64_t timestamp;
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
	int slots_used;
};

struct rv_state {
	uint64_t regs[RV_NUM_REGS];
	uint64_t pc;

	uint8_t stack[STACK_SIZE];

	bool analyze;
	bool simulate_cache;
	bool verbose;

	struct rv_analysis analysis;
	struct cache icache;
};


void rv_init(struct rv_state *state, uint32_t *target,
	     uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3);
uint64_t rv_emulate(struct rv_state *state);

void cache_init(struct cache *cache);
uint32_t cache_lookup(struct cache *cache, uint64_t addr);
void cache_print(struct cache *cache, char *name);

/* Logging */
void verbose(char *fmt, ...);
void verbose_cache(int mem_type, char *fmt, ...);
