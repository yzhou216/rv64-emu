/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>
#include "bits.h"

uint32_t get_bits(uint64_t value, uint32_t start, uint32_t count)
{
	uint64_t mask = (1 << count) - 1;
	return (value >> start) & mask;
}

bool get_bit(uint64_t value, uint32_t which)
{
	return (value >> which) & 0b1;
}
