/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "progs.h"

/* Return the value of a digit in the given base, or zero for a non-digit */
static uint32_t char_to_digit(char ch, int base)
{
	int digit;

	switch (ch) {
	case '0' ... '9':
		digit = ch - '0';
		break;
	case 'a' ... 'f':
		digit = ch - 'a' + 0xa;
		break;
	case 'A' ... 'F':
		digit = ch - 'A' + 0xa;
		break;
	default:
		return 0;
	}
	return digit < base ? digit : 0;
}

int str_to_int_c(const char *str, int base)
{
	int value = 0;
	int place = 1;
	int digit;

	for (int i = strlen(str) - 1; i >= 0; i--) {
		digit = char_to_digit(str[i], base);
		value += digit * place;
		place *= base;
	}
	return value;
}
