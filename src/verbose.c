/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* verbose.c - Implement a logging function for debugging */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "emu/rv_emu.h"

bool verbose_on = false;

void verbose(const char *fmt, ...)
{
	if (verbose_on) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}
