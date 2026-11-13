/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* getopt() and getsubopt(), which ISO C leaves out */
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "harness.h"
#include "emu/rv_emu.h"
#include "log.h"

/* Whether to print the dynamic instruction mix, which -a turns on */
static bool analyze;

[[noreturn]] static void usage(void)
{
	fputs("usage: rv64_emu [-av] [-d size=<words>,block=<words>]\n"
	      "                [-s size=<words>,block=<words>,ways=<n>]\n"
	      "                <prog> [<arg> ...]\n"
	      "\n"
	      "programs:\n", stderr);
	print_tests(stderr);
	exit(EXIT_FAILURE);
}

/* Parse a decimal int, or return false if arg is not one */
static bool parse_int(const char *arg, int *value)
{
	char *end;

	errno = 0;
	long n = strtol(arg, &end, 10);
	if (end == arg || *end || errno || n < INT_MIN || n > INT_MAX)
		return false;
	*value = n;
	return true;
}

/* Parameters of the cache, which -d and -s take as suboptions */
enum param { SIZE, BLOCK, WAYS, NPARAMS };

static char *const params[] = {
	[SIZE] = "size",
	[BLOCK] = "block",
	[WAYS] = "ways",
	[NPARAMS] = nullptr,
};

/*
 * Configure a cache of the given type from the suboptions in arg: size
 * and block for both types, and ways for a set-associative one.
 */
static void parse_cache(struct cache *cache, enum cache_type type, char *arg)
{
	int *fields[NPARAMS] = {
		[SIZE] = &cache->size,
		[BLOCK] = &cache->block_size,
		[WAYS] = &cache->ways,
	};
	bool given[NPARAMS] = {};
	char *value;

	cache->type = type;
	cache->ways = 1;
	while (*arg) {
		int i = getsubopt(&arg, params, &value);

		if (i < 0 || (i == WAYS && type != CACHE_SA) || !value ||
		    !parse_int(value, fields[i]))
			usage();
		given[i] = true;
	}
	if (!given[SIZE] || !given[BLOCK] || (type == CACHE_SA && !given[WAYS]))
		usage();
}

/*
 * Parse the options, which must precede the name of the emulated program
 * and its arguments, and return the index of that name in argv.
 */
static int parse_params(int argc, char **argv, struct rv_state *state)
{
	int opt;

	/*
	 * The + keeps the getopt() of glibc from permuting argv, which POSIX
	 * getopt() never does, so that the arguments of the emulated program,
	 * such as the -o of int_to_str and negative numbers, stay its own.
	 * Errors print the usage instead of a message with the path in argv[0].
	 */
	opterr = 0;
	while ((opt = getopt(argc, argv, "+ad:s:v")) != -1) {
		switch (opt) {
		case 'a':
			analyze = true;
			break;
		case 'd':
			parse_cache(&state->icache, CACHE_DM, optarg);
			break;
		case 's':
			parse_cache(&state->icache, CACHE_SA, optarg);
			break;
		case 'v':
			verbose_on = true;
			break;
		default:
			usage();
		}
	}

	if (state->icache.type != CACHE_NONE && !cache_valid(&state->icache))
		fatal("cannot simulate a %d-way cache of %d words in %d-word blocks",
		      state->icache.ways, state->icache.size,
		      state->icache.block_size);
	return optind;
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
