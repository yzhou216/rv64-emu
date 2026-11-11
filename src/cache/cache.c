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
		if (cache->slots[i].valid)
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
	/* The value lines up with the others because %% prints one character */
	printf("%% Used        = %.2f%%\n",
	       ((double)used / (double)cache->size) * 100.0);
}

/*
 * Replace the contents of a slot with the block of memory that holds the
 * word at addr_word, the address divided by four, and return that word,
 * which is word offset of the block.
 */
static uint32_t cache_fill_block(struct cache *cache, struct cache_slot *slot,
				 uint64_t addr_word, uint64_t offset)
{
	const uint32_t *block = (const uint32_t *)((addr_word - offset) << 2);

	for (int i = 0; i < cache->block_size; i++)
		slot->block[i] = block[i];
	return slot->block[offset];
}

/* Direct-mapped lookup */
static uint32_t cache_lookup_dm(struct cache *cache, uint64_t addr)
{
	uint64_t addr_word = addr >> 2;
	uint64_t offset = addr_word % cache->block_size;
	uint64_t index = (addr >> (cache->block_bits + 2)) & cache->index_mask;
	uint64_t tag = addr >> (cache->index_bits + cache->block_bits + 2);
	struct cache_slot *slot = &cache->slots[index];

	cache->refs += 1;
	if (slot->valid && slot->tag == tag) {
		cache->hits += 1;
		verbose("  cache tag hit for index %w64u tag %w64X addr %w64X\n",
			index, tag, addr);
		return slot->block[offset];
	}

	cache->misses += 1;
	if (slot->valid) {
		cache->misses_hot += 1;
		verbose("  cache tag (%w64X) miss for index %w64u tag %w64X addr %w64X (hot)\n",
			slot->tag, index, tag, addr);
	} else {
		cache->misses_cold += 1;
		verbose("  cache tag (%w64X) miss for index %w64u tag %w64X addr %w64X (cold)\n",
			slot->tag, index, tag, addr);
	}
	slot->valid = true;
	slot->tag = tag;
	return cache_fill_block(cache, slot, addr_word, offset);
}

/* Return the slot of a set that holds tag, or nullptr if none does */
static struct cache_slot *cache_find_tag(struct cache_slot *set, int ways,
					 uint64_t tag)
{
	for (int way = 0; way < ways; way++)
		if (set[way].valid && set[way].tag == tag)
			return &set[way];
	return nullptr;
}

/* Return the last invalid slot of a set, or nullptr if the set is full */
static struct cache_slot *cache_find_invalid(struct cache_slot *set, int ways)
{
	struct cache_slot *invalid = nullptr;

	for (int way = 0; way < ways; way++)
		if (!set[way].valid)
			invalid = &set[way];
	return invalid;
}

/* Return the least recently used slot of a set */
static struct cache_slot *cache_find_lru(struct cache_slot *set, int ways)
{
	struct cache_slot *lru = &set[0];

	for (int way = 1; way < ways; way++)
		if (set[way].timestamp < lru->timestamp)
			lru = &set[way];
	return lru;
}

/* Set-associative lookup with least recently used replacement */
static uint32_t cache_lookup_sa(struct cache *cache, uint64_t addr)
{
	/* A cache without slots misses every time */
	if (cache->size == 0) {
		cache->misses += 1;
		return *((uint32_t *)addr);
	}

	cache->refs += 1;

	uint64_t tag = addr >> (cache->index_bits + cache->block_bits + 2);
	uint64_t addr_word = addr >> 2;
	uint64_t offset = addr_word % cache->block_size;
	int set_index = (addr >> (cache->block_bits + 2)) & cache->index_mask;
	int set_base = set_index * cache->ways;
	struct cache_slot *set = &cache->slots[set_base];

	if (struct cache_slot *hit = cache_find_tag(set, cache->ways, tag)) {
		verbose("  cache tag hit for set %d way %td tag %w64X addr %w64X\n",
			set_index, hit - set, tag, addr);
		cache->hits++;
		hit->timestamp = cache->refs;
		return hit->block[offset];
	}

	cache->misses += 1;
	struct cache_slot *victim = cache_find_invalid(set, cache->ways);
	if (victim) {
		cache->misses_cold += 1;
		verbose("  cache tag (%w64X) miss for set %d tag %w64X addr %w64X (fill invalid slot)\n",
			victim->tag, set_index, tag, addr);
	} else {
		victim = cache_find_lru(set, cache->ways);
		cache->misses_hot += 1;
		verbose("  cache tag (%w64X) miss for set %d tag %w64X addr %w64X (evict address %w64X)\n",
			victim->tag, set_index, tag, addr,
			victim->tag << (cache->index_bits + 2) |
			set_index << 2);
	}

	uint32_t value = cache_fill_block(cache, victim, addr_word, offset);
	victim->tag = tag;
	victim->valid = true;
	victim->timestamp = cache->refs;
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
