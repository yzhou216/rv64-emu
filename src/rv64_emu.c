/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "emu/rv_emu.h"
#include "log.h"

/* Whether to print the dynamic instruction mix, which -a turns on */
static bool analyze;

[[noreturn]] static void usage(void)
{
	fputs("usage: rv64_emu [-a] [-v] [-dm <size> <block>] [-sa <size> <block> <ways>]\n"
	      "                <prog> [<arg> ...]\n"
	      "\n"
	      "programs:\n", stderr);
	print_tests(stderr);
	exit(EXIT_FAILURE);
}

/*
 * Parse the options, which must precede the name of the emulated program
 * and its arguments, and return the index of that name in argv.
 */
static int parse_params(int argc, char **argv, struct rv_state *state)
{
	int i;

	for (i = 1; i < argc && argv[i][0] == '-'; i++) {
		if (!strcmp(argv[i], "-a")) {
			analyze = true;
		} else if (!strcmp(argv[i], "-dm") && i + 2 < argc) {
			state->icache.type = CACHE_DM;
			state->icache.size = atoi(argv[i + 1]);
			state->icache.block_size = atoi(argv[i + 2]);
			state->icache.ways = 1;
			i += 2;
		} else if (!strcmp(argv[i], "-sa") && i + 3 < argc) {
			state->icache.type = CACHE_SA;
			state->icache.size = atoi(argv[i + 1]);
			state->icache.block_size = atoi(argv[i + 2]);
			state->icache.ways = atoi(argv[i + 3]);
			i += 3;
		} else if (!strcmp(argv[i], "-v")) {
			verbose_on = true;
		} else {
			usage();
		}
	}

	if (state->icache.type != CACHE_NONE && !cache_valid(&state->icache))
		fatal("cannot simulate a %d-way cache of %d words in %d-word blocks",
		      state->icache.ways, state->icache.size,
		      state->icache.block_size);
	return i;
}

int main(int argc, char **argv)
{
	struct rv_state state = {};
	int i = parse_params(argc, argv, &state);
	int nargs = argc - i - 1;

	if (i >= argc || !run_test(argv[i], nargs, &argv[i + 1], &state))
		usage();

	if (analyze)
		rv_print(&state.analysis);

	if (state.icache.type != CACHE_NONE)
		cache_print(&state.icache, "(I)");

	return 0;
}
