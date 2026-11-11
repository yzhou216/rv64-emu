/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

int find_max_index_c(int *values, int len);

void sort_c(int *values, int len)
{
	for (int i = 0; i < len; i++) {
		int max = find_max_index_c(&values[i], len - i) + i;
		if (values[max] > values[i]) {
			int tmp = values[i];
			values[i] = values[max];
			values[max] = tmp;
		}
	}
}
