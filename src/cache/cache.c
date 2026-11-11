/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdbit.h>
#include <stdcountof.h>
#include <stdio.h>

#include "cache.h"
#include "log.h"

/*
 * Return whether the simulator can model the cache: size words in blocks
 * of block_size words, grouped into sets of ways slots.  The masks in
 * cache_init() need a power of two for both the block size and the number
 * of sets, and all slots have to fit.
 */
bool cache_valid(const struct cache *cache)
{
	if (cache->block_size < 1 || cache->block_size > CACHE_MAX_BLOCK_SIZE ||
	    !stdc_has_single_bit((unsigned int)cache->block_size) ||
	    cache->ways < 1 || cache->size < 1 ||
	    cache->size % cache->block_size)
		return false;

	int slots = cache->size / cache->block_size;
	if (slots > (int)countof(cache->slots) || slots % cache->ways)
		return false;
	return stdc_has_single_bit((unsigned int)(slots / cache->ways));
}

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

	cache->stats = (struct cache_stats){};

	verbose("Cache initialized.\n");
}

void cache_print(const struct cache *cache, const char *name)
{
	const struct cache_stats *stats = &cache->stats;
	int slots = cache->size / cache->block_size;
	uint64_t misses = stats->cold + stats->hot;
	int used = 0;

	for (int i = 0; i < slots; i++) {
		if (cache->slots[i].valid)
			used++;
	}

	printf("=== Cache %s\n", name);
	printf("Type          = ");
	if (cache->type == CACHE_DM)
		printf("direct mapped\n");
	else if (cache->type == CACHE_SA)
		printf("set associative\n");

	printf("Size          = %d words\n", cache->size);
	printf("Block size    = %d words\n", cache->block_size);
	printf("Ways          = %d\n", cache->ways);
	printf("References    = %w64u\n", stats->refs);
	printf("Hits          = %w64u (%.2f%% hit ratio)\n", stats->hits,
	       ((double)stats->hits / (double)stats->refs) * 100.00);
	printf("Misses        = %w64u (%.2f%% miss ratio)\n", misses,
	       ((double)misses / (double)stats->refs) * 100.00);
	printf("Misses (cold) = %w64u\n", stats->cold);
	printf("Misses (hot)  = %w64u\n", stats->hot);
	/* The value lines up with the others because %% prints one character */
	printf("%% Used        = %.2f%%\n", (double)used / (double)slots * 100.0);
}

/*
 * Where an address falls in the cache: the tag that identifies its block,
 * the index of the slot or the set that holds the block, and the offset
 * of the word within the block.
 */
struct cache_addr {
	uint64_t tag;
	uint64_t index;
	uint64_t offset;
};

static struct cache_addr cache_split(const struct cache *cache, uint64_t addr)
{
	uint64_t word = addr >> 2;

	return (struct cache_addr){
		.tag = word >> (cache->block_bits + cache->index_bits),
		.index = (word >> cache->block_bits) & cache->index_mask,
		.offset = word & cache->block_mask,
	};
}

/*
 * Replace the contents of a slot with the block of memory that holds the
 * word at addr, which is word offset of the block, and return that word.
 */
static uint32_t cache_fill_block(struct cache *cache, struct cache_slot *slot,
				 uint64_t addr, uint64_t offset)
{
	const uint32_t *block = (const uint32_t *)addr - offset;

	for (int i = 0; i < cache->block_size; i++)
		slot->block[i] = block[i];
	return slot->block[offset];
}

/* Direct-mapped lookup */
static uint32_t cache_lookup_dm(struct cache *cache, uint64_t addr)
{
	auto pos = cache_split(cache, addr);
	struct cache_slot *slot = &cache->slots[pos.index];

	cache->stats.refs += 1;
	if (slot->valid && slot->tag == pos.tag) {
		cache->stats.hits += 1;
		verbose("  cache tag hit for index %w64u tag %w64X addr %w64X\n",
			pos.index, pos.tag, addr);
		return slot->block[pos.offset];
	}

	if (slot->valid) {
		cache->stats.hot += 1;
		verbose("  cache tag (%w64X) miss for index %w64u tag %w64X addr %w64X (hot)\n",
			slot->tag, pos.index, pos.tag, addr);
	} else {
		cache->stats.cold += 1;
		verbose("  cache tag (%w64X) miss for index %w64u tag %w64X addr %w64X (cold)\n",
			slot->tag, pos.index, pos.tag, addr);
	}
	slot->valid = true;
	slot->tag = pos.tag;
	return cache_fill_block(cache, slot, addr, pos.offset);
}

/* Return the slot of a set that holds tag, or nullptr if none does */
static struct cache_slot *cache_find_tag(struct cache_slot *set, int ways,
					 uint64_t tag)
{
	for (int way = 0; way < ways; way++) {
		if (set[way].valid && set[way].tag == tag)
			return &set[way];
	}
	return nullptr;
}

/* Return the last invalid slot of a set, or nullptr if the set is full */
static struct cache_slot *cache_find_invalid(struct cache_slot *set, int ways)
{
	struct cache_slot *invalid = nullptr;

	for (int way = 0; way < ways; way++) {
		if (!set[way].valid)
			invalid = &set[way];
	}
	return invalid;
}

/* Return the least recently used slot of a set */
static struct cache_slot *cache_find_lru(struct cache_slot *set, int ways)
{
	struct cache_slot *lru = &set[0];

	for (int way = 1; way < ways; way++) {
		if (set[way].timestamp < lru->timestamp)
			lru = &set[way];
	}
	return lru;
}

/* Set-associative lookup with least recently used replacement */
static uint32_t cache_lookup_sa(struct cache *cache, uint64_t addr)
{
	auto pos = cache_split(cache, addr);
	struct cache_slot *set = &cache->slots[pos.index * cache->ways];

	cache->stats.refs += 1;
	if (struct cache_slot *hit = cache_find_tag(set, cache->ways,
						    pos.tag)) {
		verbose("  cache tag hit for set %w64u way %td tag %w64X addr %w64X\n",
			pos.index, hit - set, pos.tag, addr);
		cache->stats.hits += 1;
		hit->timestamp = cache->stats.refs;
		return hit->block[pos.offset];
	}

	struct cache_slot *victim = cache_find_invalid(set, cache->ways);
	if (victim) {
		cache->stats.cold += 1;
		verbose("  cache tag (%w64X) miss for set %w64u tag %w64X addr %w64X (fill invalid slot)\n",
			victim->tag, pos.index, pos.tag, addr);
	} else {
		victim = cache_find_lru(set, cache->ways);
		cache->stats.hot += 1;
		verbose("  cache tag (%w64X) miss for set %w64u tag %w64X addr %w64X (evict address %w64X)\n",
			victim->tag, pos.index, pos.tag, addr,
			victim->tag << (cache->index_bits +
					cache->block_bits + 2) |
			pos.index << (cache->block_bits + 2));
	}

	uint32_t value = cache_fill_block(cache, victim, addr, pos.offset);
	victim->tag = pos.tag;
	victim->valid = true;
	victim->timestamp = cache->stats.refs;
	return value;
}

/* Fetch the instruction word at addr through the simulated cache, if any */
uint32_t cache_lookup(struct cache *cache, uint64_t addr)
{
	switch (cache->type) {
	case CACHE_DM:
		return cache_lookup_dm(cache, addr);
	case CACHE_SA:
		return cache_lookup_sa(cache, addr);
	case CACHE_NONE:
		break;
	}
	return *(const uint32_t *)addr;
}
