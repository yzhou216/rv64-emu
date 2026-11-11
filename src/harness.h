/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* Runs of the test programs in C, in assembly, and in the emulator */

#ifndef HARNESS_H
#define HARNESS_H

#include <stdio.h>

#include "emu/rv_emu.h"

[[nodiscard]] bool run_test(const char *name, int argc, char **argv,
			    struct rv_state *state);
void print_tests(FILE *stream);

#endif /* HARNESS_H */
