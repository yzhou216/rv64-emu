/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"

extern bool verbose_on;

void usage()
{
	printf("usage: rv64_emu <prog> [<arg1> ...]\n");
	exit(-1);
}

int parse_params(int argc, char **argv, struct rv_state *state)
{
	/*
	 * Parse optional command line parameters which must precede
	 * the name of the emulated program and its parameters
	 */
	state->analyze = false;
	state->simulate_cache = false;
	state->icache.type = CACHE_NONE;

	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			/*
			 * Return the index of the first non-optional parameter,
			 * which is the name of the emulated program
			 */
			break;
		} else if (!strcmp(argv[i], "-a")) {
			state->analyze = true;
		} else if (!strcmp(argv[i], "-dm")) {
			state->simulate_cache = true;
			state->icache.type = CACHE_DM;
			state->icache.size = atoi(argv[i + 1]);
			state->icache.block_size = atoi(argv[i + 2]);
			state->icache.ways = 1;
			i += 2;
		} else if (!strcmp(argv[i], "-sa")) {
			state->simulate_cache = true;
			state->icache.type = CACHE_SA;
			state->icache.size = atoi(argv[i + 1]);
			state->icache.block_size = atoi(argv[i + 2]);
			state->icache.ways = atoi(argv[i + 3]);
			i += 3;
		} else if (!strcmp(argv[i], "-v")) {
			state->verbose = true;
			verbose_on = true;
		}
	}
	return i;
}

int main(int argc, char **argv)
{
	if (argc == 1)
		usage();

	struct rv_state state;
	int i = parse_params(argc, argv, &state);

	if (!run_test(argv[i], i + 1, argc, argv, &state))
		usage();

	if (state.analyze)
		rv_print(&state.analysis);

	if (state.simulate_cache)
		cache_print(&state.icache, "(I)");

	return 0;
}
