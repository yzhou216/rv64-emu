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

	cache->index_bits = 0;
	while (cache->index_mask & (1 << cache->index_bits))
		cache->index_bits++;

	cache->block_bits = 0;
	while (cache->block_mask & (1 << cache->block_bits))
		cache->block_bits++;

	for (int i = 0; i < CACHE_MAX_SLOTS; i++) {
		cache->slots[i].valid = 0;
		cache->slots[i].tag = 0;
		for (int j = 0; j < CACHE_MAX_BLOCK_SIZE; j++)
			cache->slots[i].block[j] = 0;
		/* timestamp only used for SA cache */
		cache->slots[i].timestamp = 0;
	}

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

/* Direct mapped lookup */
uint32_t cache_lookup_dm(struct cache *cache, uint64_t addr)
{
	uint64_t tag;
	uint64_t index;
	uint64_t offset;
	uint64_t base;
	struct cache_slot *slot;
	uint32_t data = 0;

	offset = 0; /* TODO: Need to change for block size > 1 */
	index = (addr >> (cache->block_bits + 2)) & cache->index_mask;
	tag = addr >> (cache->index_bits + cache->block_bits + 2);

	slot = &cache->slots[index];

	cache->refs += 1;
	if (slot->valid && (slot->tag == tag)) {
		/* hit */
		cache->hits += 1;
		data = slot->block[offset];

		verbose("  cache tag hit for index %d tag %X addr %lX\n",
			index, tag, addr);
	} else {
		/* miss */
		cache->misses += 1;
		if (slot->valid == 0) {
			cache->misses_cold += 1;
			verbose
			    ("  cache tag (%X) miss for index %d tag %X addr %X (cold)\n",
			     slot->tag, index, tag, addr);
		} else {
			cache->misses_hot += 1;
			verbose
			    ("  cache tag (%X) miss for index %d tag %X addr %X (hot)\n",
			     slot->tag, index, tag, addr);
		}
		slot->valid = 1;
		slot->tag = tag;

		/* TODO: Need to change for block size > 1 */
		data = slot->block[offset];
	}

	return data;
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

	uint64_t offset = 1;	/* Need to change for block size > 1 */

	uint64_t base;
	int set_index = (addr >> (cache->block_bits + 2)) & cache->index_mask;
	int set_base = set_index * cache->ways;

	struct cache_slot *slot = NULL;
	struct cache_slot *found = NULL;
	struct cache_slot *invalid = NULL;

	/* Check each slot in the set */
	for (int i = 0; i < 4; i += 1) {
		slot = &cache->slots[set_base + i];
		if (slot->valid) {
			if (tag == slot->tag) {
				verbose
				    ("  cache tag hit for set %d way %d tag %X addr %lX\n",
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
			verbose
			    ("  cache tag (%X) miss for set %d tag %X addr %X (fill invalid slot)\n",
			     slot->tag, set_index, tag, addr);
			cache->misses += 1;
			/* Miss due to tag collision is a "hot" miss */
			cache->misses_cold += 1;
		} else {
			/* Always pick first slot in set - CHANGE TO LRU */
			slot = &(cache->slots[set_base]);

			verbose
			    ("  cache tag (%X) miss for set %d tag %X addr %X (evict address %X)\n",
			     slot->tag, set_index, tag, addr,
			     ((slot->
			       tag << (cache->index_bits +
				       2)) | (set_index << 2)));

			cache->misses += 1;
			/* Miss due to tag collision is a "hot" miss */
			cache->misses_hot += 1;
		}
	}

	if (!hit) {
		/* Need to change for block size > 1 */
		slot->block[offset] = *((uint32_t *)addr);
		slot->tag = tag;
		slot->valid = true;
	}

	value = slot->block[offset];

	slot->timestamp = cache->refs;
	return value;
}

/* Cache lookup */
uint32_t cache_lookup(struct cache *cache, uint64_t addr)
{
	uint32_t data;
	if (cache->type == CACHE_DM)
		data = cache_lookup_dm(cache, addr);
	else if (cache->type == CACHE_SA)
		data = cache_lookup_sa(cache, addr);
	else
		data = *((uint32_t *)addr);
	return data;
}
