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

int64_t sign_extend(uint64_t value, uint32_t width)
{
	uint32_t shift = 64 - width;
	uint64_t shifted = value << shift;
	int64_t extended = ((int64_t) shifted) >> shift;
	return extended;
}

bool get_bit(uint64_t value, uint32_t which)
{
	return (value >> which) & 0b1;
}
