/*
 * SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* Instruction cache simulator */

#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>

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

/* What the lookups found */
struct cache_stats {
	uint64_t refs;
	uint64_t hits;
	uint64_t cold; /* Misses that filled an invalid slot */
	uint64_t hot; /* Misses that evicted a block */
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
	struct cache_stats stats;
};

[[nodiscard]] bool cache_valid(const struct cache *cache);
void cache_init(struct cache *cache);
[[nodiscard]] uint32_t cache_lookup(struct cache *cache, uint64_t addr);
void cache_print(const struct cache *cache, const char *name);

#endif /* CACHE_H */
