/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

constexpr int MAX_DIGITS = 34;

void int_to_str_c(uint32_t value, char *out, int base)
{
	uint32_t quotient, digit;
	int i, j = 0, len;
	char digits[MAX_DIGITS];

	len = 0;
	if (value == 0) {
		digits[len++] = '0';
	} else {
		while (value != 0) {
			quotient = value / base;
			digit = value % base;
			if (digit <= 9)
				digits[len] = '0' + digit;
			else if (digit >= 10 && digit <= 15)
				digits[len] = 'a' - 0xa + digit;
			len += 1;
			value = quotient;
		}
	}

	/* Put base prefix into result string */
	if (base == 2) {
		out[j] = '0';
		out[j + 1] = 'b';
		j += 2;
	} else if (base == 16) {
		out[j] = '0';
		out[j + 1] = 'x';
		j += 2;
	}

	/* Reverse digits and put into result string */
	for (i = len; i > 0; i--, j++) {
		out[j] = digits[i - 1];
	}
	out[j] = '\0';
}
