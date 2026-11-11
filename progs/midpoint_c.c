/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "progs.h"

int midpoint_c(int start, int end)
{
	/* The sum of two ints can overflow an int, but not an int64_t */
	return ((int64_t)start + end) / 2;
}
