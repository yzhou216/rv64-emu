/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

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

void fatal(const char *fmt, ...)
{
	va_list args;

	/* Keep the output of the runs before the error in order */
	fflush(stdout);
	fputs("rv64_emu: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}
