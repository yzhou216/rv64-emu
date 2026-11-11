/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdcountof.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "log.h"
#include "progs.h"

constexpr int MAX_ARRAY = 1024;

static void copy_array(int *dst, const int *src, int len)
{
	for (int i = 0; i < len; i++)
		dst[i] = src[i];
}

static void print_array(const char *prefix, const int *values, int len)
{
	printf("%s", prefix);
	for (int i = 0; i < len; i++)
		printf(" %d", values[i]);
	printf("\n");
}

static void print_results(const char *prefix, const uint64_t *results, int n)
{
	printf("%s", prefix);
	for (int i = 0; i < n; i++)
		printf(" %w64x", results[i]);
	printf("\n");
}

static bool fib_rec_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	int result;
	int n = atoi(argv[0]);

	result = fib_rec_c(n);
	printf("C: %d\n", result);

	result = fib_rec_s(n);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)fib_rec_s, n, 0, 0, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
	return true;
}

static bool get_bitseq_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	int result;
	int n = atoi(argv[0]);
	int start = atoi(argv[1]);
	int end = atoi(argv[2]);

	result = get_bitseq_c(n, start, end);
	printf("C: %d\n", result);

	result = get_bitseq_s(n, start, end);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)get_bitseq_s, n, start, end, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
	return true;
}

static bool pal_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	bool result;
	const char *str = argv[0];
	int end = strlen(str) - 1;

	result = pal_rec_c(str, 0, end);
	printf("C: %s\n", result ? "is a palindrome" : "is not a palindrome");

	result = pal_rec_s(str, 0, end);
	printf("Asm: %s\n", result ? "is a palindrome" : "is not a palindrome");

	rv_init(state, (uint32_t *)pal_rec_s, (uint64_t)str, 0, end, 0);
	result = rv_emulate(state);
	printf("Emu: %s\n", result ? "is a palindrome" : "is not a palindrome");
	return true;
}

static bool int_to_str_test(int argc, char **argv, struct rv_state *state)
{
	char out[MAX_ARRAY];

	int value = atoi(argv[0]);
	int base = 10;
	if (argc == 3 && !strcmp(argv[1], "-o"))
		base = atoi(argv[2]);
	else if (argc != 1)
		return false;

	memset(out, 0, sizeof(out));
	int_to_str_c(value, out, base);
	printf("C: %s\n", out);

	memset(out, 0, sizeof(out));
	int_to_str_s(value, out, base);
	printf("Asm: %s\n", out);

	memset(out, 0, sizeof(out));
	rv_init(state, (uint32_t *)int_to_str_s, value, (uint64_t)out,
		base, 0);
	rv_emulate(state);
	printf("Emu: %s\n", out);
	return true;
}

static bool max3_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	int result;
	int a = atoi(argv[0]);
	int b = atoi(argv[1]);
	int c = atoi(argv[2]);

	result = max3_c(a, b, c);
	printf("C: %d\n", result);

	result = max3_s(a, b, c);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)max3_s, a, b, c, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
	return true;
}

static bool midpoint_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	int result;
	int start = atoi(argv[0]);
	int end = atoi(argv[1]);

	result = midpoint_c(start, end);
	printf("C: %d\n", result);

	result = midpoint_s(start, end);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)midpoint_s, start, end, 0, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
	return true;
}

static bool quadratic_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	int result;
	int x = atoi(argv[0]);
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	int c = atoi(argv[3]);

	result = quadratic_c(x, a, b, c);
	printf("C: %d\n", result);

	result = quadratic_s(x, a, b, c);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)quadratic_s, x, a, b, c);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
	return true;
}

static bool sort_test(int argc, char **argv, struct rv_state *state)
{
	int sorted[MAX_ARRAY];
	int values[MAX_ARRAY] = {};
	int len = 0;

	for (int i = 0; i < argc; i++) {
		values[i] = atoi(argv[i]);
		len++;
	}

	copy_array(sorted, values, len);
	sort_c(sorted, len);
	print_array("C:", sorted, len);

	copy_array(sorted, values, len);
	sort_s(sorted, len);
	print_array("Asm:", sorted, len);

	copy_array(sorted, values, len);
	rv_init(state, (uint32_t *)sort_s, (uint64_t)sorted, len, 0, 0);
	rv_emulate(state);
	print_array("Emu:", sorted, len);
	return true;
}

static bool str_to_int_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	const char *str = argv[0];
	int base = 10;
	if (!strncmp(str, "0b", 2)) {
		base = 2;
		str += 2;
	} else if (!strncmp(str, "0x", 2)) {
		base = 16;
		str += 2;
	}

	int result = str_to_int_c(str, base);
	printf("C: %d\n", result);

	result = str_to_int_s(str, base);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)str_to_int_s, (uint64_t)str, base, 0, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
	return true;
}

static bool to_upper_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	/* Make a copy of the string since we modify it in place */
	char out[MAX_ARRAY];
	const char *str = argv[0];

	if (strlen(str) >= countof(out))
		fatal("to_upper takes at most %zu characters", countof(out) - 1);

	strcpy(out, str);
	to_upper_c(str, out);
	printf("C: %s\n", out);

	strcpy(out, str);
	to_upper_s(str, out);
	printf("Asm: %s\n", out);

	strcpy(out, str);
	rv_init(state, (uint32_t *)to_upper_s, (uint64_t)str, (uint64_t)out,
		0, 0);
	rv_emulate(state);
	printf("Emu: %s\n", out);
	return true;
}

/*
 * insn_s has no C version, so compare its native results with the
 * emulated ones only.
 */
static bool insn_test([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
		      struct rv_state *state)
{
	uint64_t results[64] = {};
	int n;

	n = insn_s(results);
	print_results("Asm:", results, n);

	memset(results, 0, sizeof(results));
	rv_init(state, (uint32_t *)insn_s, (uint64_t)results, 0, 0, 0);
	n = rv_emulate(state);
	print_results("Emu:", results, n);
	return true;
}

/*
 * Test programs by the name that selects them on the command line, with
 * a summary of their arguments and the range of their argument count.
 * run() returns false, before it prints anything, if it rejects its
 * arguments.
 */
static const struct test {
	const char *name;
	const char *args;
	int min_args;
	int max_args;
	bool (*run)(int argc, char **argv, struct rv_state *state);
} tests[] = {
	{ "fib_rec", "<n>", 1, 1, fib_rec_test },
	{ "get_bitseq", "<n> <start> <end>", 3, 3, get_bitseq_test },
	{ "insn", "", 0, 0, insn_test },
	{ "int_to_str", "<n> [-o <base>]", 1, 3, int_to_str_test },
	{ "is_pal", "<string>", 1, 1, pal_test },
	{ "max3", "<a> <b> <c>", 3, 3, max3_test },
	{ "midpoint", "<start> <end>", 2, 2, midpoint_test },
	{ "quadratic", "<x> <a> <b> <c>", 4, 4, quadratic_test },
	{ "sort", "<n> ...", 0, MAX_ARRAY, sort_test },
	{ "str_to_int", "<string>", 1, 1, str_to_int_test },
	{ "to_upper", "<string>", 1, 1, to_upper_test },
};

static const struct test *find_test(const char *name)
{
	for (size_t i = 0; i < countof(tests); i++)
		if (!strcmp(tests[i].name, name))
			return &tests[i];
	return nullptr;
}

/*
 * Run the test program called name with the argc arguments in argv, and
 * return false if there is no such program or it rejects the arguments.
 */
bool run_test(const char *name, int argc, char **argv,
	      struct rv_state *state)
{
	if (const struct test *test = find_test(name);
	    test && argc >= test->min_args && argc <= test->max_args)
		return test->run(argc, argv, state);
	return false;
}

/* List the test programs and their arguments for the usage message */
void print_tests(FILE *stream)
{
	for (size_t i = 0; i < countof(tests); i++) {
		if (*tests[i].args)
			fprintf(stream, "  %-10s %s\n", tests[i].name,
				tests[i].args);
		else
			fprintf(stream, "  %s\n", tests[i].name);
	}
}
