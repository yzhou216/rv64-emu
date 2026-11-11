/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "progs.h"

int quadratic_c(int x, int a, int b, int c)
{
	uint32_t ux = x;

	/* Unsigned arithmetic wraps around where signed arithmetic overflows */
	return (uint32_t)a * ux * ux + (uint32_t)b * ux + (uint32_t)c;
}
