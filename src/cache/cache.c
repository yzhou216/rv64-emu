/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdbit.h>
#include <stdcountof.h>
#include <stdio.h>
#include <stdlib.h>

#include "emu/rv_emu.h"

void cache_init(struct cache *cache)
{
	if (cache->type == CACHE_NONE)
		return;

	cache->block_mask = (cache->block_size) - 1;
	cache->index_mask =
		((cache->size / cache->block_size) / cache->ways) - 1;

	cache->index_bits = stdc_trailing_ones(cache->index_mask);
	cache->block_bits = stdc_trailing_ones(cache->block_mask);

	for (size_t i = 0; i < countof(cache->slots); i++)
		cache->slots[i] = (struct cache_slot){};

	cache->refs = 0;
	cache->hits = 0;
	cache->misses = 0;
	cache->misses_cold = 0;
	cache->misses_hot = 0;

	verbose("Cache initialized.\n");
}

void cache_print(struct cache *cache, char *name)
{
	int used = 0;
	int i;

	for (i = 0; i < cache->size; i++)
		if (cache->slots[i].valid == 1)
			used += 1;

	printf("=== Cache %s\n", name);
	printf("Type          = ");
	if (cache->type == CACHE_DM)
		printf("direct mapped\n");
	else if (cache->type == CACHE_SA)
		printf("set associative\n");

	printf("Size          = %d slots\n", cache->size);
	printf("Block size    = %d words\n", cache->block_size);
	printf("Ways          = %d\n", cache->ways);
	printf("References    = %d\n", cache->refs);
	printf("Hits          = %d (%.2f%% hit ratio)\n", cache->hits,
	       ((double)cache->hits / (double)cache->refs) * 100.00);
	printf("Misses        = %d (%.2f%% miss ratio)\n", cache->misses,
	       ((double)cache->misses / (double)cache->refs) * 100.00);
	printf("Misses (cold) = %d\n", cache->misses_cold);
	printf("Misses (hot)  = %d\n", cache->misses_hot);
	/* TODO: Funky format due to bad test cases. Remove later */
	printf("%% Used        = %.2f%%\n",
	       ((double)used / (double)cache->size) * 100.0);
}

uint32_t evict_data_in_cache_block(struct cache *cache, struct cache_slot *slot,
				   uint64_t addr_word, uint64_t offset)
{
		uint64_t block_base = addr_word - offset;
		/* Convert block base to a pointer to the word in memory */
		uint32_t *src = (uint32_t *)(block_base << 2);

		/*
		 * Evict old data from the block and update with new data
		 * retrieved from the memory bus
		 */
                for (int i = 0; i < cache->block_size; i++) {
                        slot->block[i] = *src;
                        src++; /* Move pointer by 32 bits (next word) */
                }

		/* Word is now in the updated cache block */
		uint32_t data = slot->block[offset];
		return data;
}

/* Direct mapped lookup */
uint32_t cache_lookup_dm(struct cache *cache, uint64_t addr)
{
	uint64_t tag;
	uint64_t index;
	uint64_t offset;
	struct cache_slot *slot;
	uint32_t data = 0;

	uint64_t addr_word = addr >> 2;
	offset = addr_word % cache->block_size; /* mask off according to the block size */

	index = (addr >> (cache->block_bits + 2)) & cache->index_mask;
	tag = addr >> (cache->index_bits + cache->block_bits + 2);

	slot = &cache->slots[index];

	cache->refs += 1;
	if (slot->valid && (slot->tag == tag)) {
		/* hit */
		cache->hits += 1;
		data = slot->block[offset];

		verbose("  cache tag hit for index %w64u tag %w64X addr %w64X\n",
			index, tag, addr);
	} else {
		/* miss */
		cache->misses += 1;
		if (slot->valid == 0) {
			cache->misses_cold += 1;
			verbose("  cache tag (%w64X) miss for index %w64u tag %w64X addr %w64X (cold)\n",
				slot->tag, index, tag, addr);
		} else {
			cache->misses_hot += 1;
			verbose("  cache tag (%w64X) miss for index %w64u tag %w64X addr %w64X (hot)\n",
				slot->tag, index, tag, addr);
		}
		slot->valid = 1;
		slot->tag = tag;

		data = evict_data_in_cache_block(cache, slot, addr_word,
						 offset);
	}

	return data;
}

struct cache_slot *find_lru_slot_in_set(struct cache *cache, uint32_t set_base)
{
	struct cache_slot *lru = &cache->slots[set_base];
	for (int i = 0; i < cache->ways; i++) {
		struct cache_slot *slot = &cache->slots[set_base + i];
		if (slot->timestamp < lru->timestamp)
			lru = slot;
	}
	return lru;
}

uint32_t cache_lookup_sa(struct cache *cache, uint64_t addr)
{
	bool hit = false;
	uint32_t value;

	/*
	 * Didn't allocate any cache. I guess that's a miss?
	 * This should not happen in our code as we don't simulate unless
	 * cache_size > 0
	 */

	if (cache->size == 0) {
		cache->misses += 1;
		return *((uint32_t *)addr);
	}

	cache->refs += 1;

	uint64_t tag = addr >> (cache->index_bits + cache->block_bits + 2);

	uint64_t addr_word = addr >> 2;
	uint64_t offset = addr_word % cache->block_size; /* mask off according to the block size */

	int set_index = (addr >> (cache->block_bits + 2)) & cache->index_mask;
	int set_base = set_index * cache->ways;

	struct cache_slot *slot = nullptr;
	struct cache_slot *found = nullptr;
	struct cache_slot *invalid = nullptr;

	/* Check each slot in the set */
	for (int i = 0; i < cache->ways; i += 1) {
		slot = &cache->slots[set_base + i];
		if (slot->valid) {
			if (tag == slot->tag) {
				verbose("  cache tag hit for set %d way %d tag %w64X addr %w64X\n",
					set_index, i, tag, addr);
				hit = true;
				found = slot;
				cache->hits++;
				break;
			}
		} else {
			/* Save invalid slot in case of miss */
			invalid = slot;
		}
	}

	if (!found) {
		if (invalid) {
			slot = invalid;
			verbose("  cache tag (%w64X) miss for set %d tag %w64X addr %w64X (fill invalid slot)\n",
				slot->tag, set_index, tag, addr);
			cache->misses += 1;
			/* Miss due to tag collision is a "hot" miss */
			cache->misses_cold += 1;
		} else {
			/* hot miss */
			slot = find_lru_slot_in_set(cache, set_base);

			verbose("  cache tag (%w64X) miss for set %d tag %w64X addr %w64X (evict address %w64X)\n",
				slot->tag, set_index, tag, addr,
				slot->tag << (cache->index_bits + 2) |
				set_index << 2);

			cache->misses += 1;
			/* Miss due to tag collision is a "hot" miss */
			cache->misses_hot += 1;
		}
	}

	if (!hit) {
		value = evict_data_in_cache_block(cache, slot, addr_word,
						  offset);
		slot->tag = tag;
		slot->valid = true;
	} else {
		value = slot->block[offset];
	}

	slot->timestamp = cache->refs;
	return value;
}

/* Cache lookup */
uint32_t cache_lookup(struct cache *cache, uint64_t addr)
{
	uint32_t data;
	switch (cache->type) {
	case CACHE_DM:
		data = cache_lookup_dm(cache, addr);
		break;
	case CACHE_SA:
		data = cache_lookup_sa(cache, addr);
		break;
	default:
		data = *((uint32_t *)addr);
		break;
	}
	return data;
}
