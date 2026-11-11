/*
 * SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* Messages for -v and fatal errors */

#ifndef LOG_H
#define LOG_H

/* Whether verbose() prints anything, which -v turns on */
extern bool verbose_on;

[[gnu::format(printf, 1, 2)]] void verbose(const char *fmt, ...);

/* Print an error after the name of the program, and exit */
[[noreturn, gnu::format(printf, 1, 2)]] void fatal(const char *fmt, ...);

#endif /* LOG_H */
