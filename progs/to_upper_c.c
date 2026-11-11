/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "progs.h"

void to_upper_c(const char *src, char *dst)
{
	char ch;
	while ((ch = *src++)) {
		if (ch >= 'a' && ch <= 'z')
			ch -= 32;
		*dst++ = ch;
	}
	*dst = '\0';
}
