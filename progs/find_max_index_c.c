/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <limits.h>

#include "progs.h"

int find_max_index_c(const int *values, int len)
{
	int max_index = 0;
	int max = INT_MIN;

	for (int i = 0; i < len; i++)
		if (values[i] > max) {
			max_index = i;
			max = values[i];
		}

	return max_index;
}
