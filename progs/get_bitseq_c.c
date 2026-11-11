/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "progs.h"

uint32_t get_bitseq_c(uint32_t value, int start, int end)
{
	uint32_t bits;
	int len;
	uint32_t mask;

	len = (end - start) + 1;
	bits = value >> start;

	if (len == 32) {
		mask = 0xFFFFFFFF;
	} else {
		mask = (0b1 << len) - 1;
	}

	bits = bits & mask;

	return bits;
}
