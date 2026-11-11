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
#include "progs.h"

constexpr int MAX_ARRAY = 1024;

static void copy_array(int *dst, int *src, int len)
{
	for (int i = 0; i < len; i++)
		dst[i] = src[i];
}

static void print_array(char *prefix, int *values, int len)
{
	printf("%s", prefix);
	for (int i = 0; i < len; i++)
		printf(" %d", values[i]);
	printf("\n");
}

static void fib_rec_test([[maybe_unused]] int argc, char **argv,
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
}

static void get_bitseq_test([[maybe_unused]] int argc, char **argv,
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
}

static void pal_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	bool result;
	char *str = argv[0];
	int end = strlen(str) - 1;

	result = pal_rec_c(str, 0, end);
	printf("C: %s\n", result ? "is a palindrome" : "is not a palindrome");

	result = pal_rec_s(str, 0, end);
	printf("Asm: %s\n", result ? "is a palindrome" : "is not a palindrome");

	rv_init(state, (uint32_t *)pal_rec_s, (uint64_t)str, 0, end, 0);
	result = rv_emulate(state);
	printf("Emu: %s\n", result ? "is a palindrome" : "is not a palindrome");
}

static void int_to_str_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	char out[MAX_ARRAY];

	int value = atoi(argv[0]);
	int base = 10;
	if (!strncmp(argv[1], "-o", 2))
		base = atoi(argv[2]);

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
}

static void max3_test([[maybe_unused]] int argc, char **argv,
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
}

static void midpoint_test([[maybe_unused]] int argc, char **argv,
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
}

static void quadratic_test([[maybe_unused]] int argc, char **argv,
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
}

static void sort_test(int argc, char **argv, struct rv_state *state)
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

}

static void str_to_int_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	char *str = argv[0];
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
}

static void to_upper_test([[maybe_unused]] int argc, char **argv,
				struct rv_state *state)
{
	/* Make a copy of the string since we modify it in place */
	char out[MAX_ARRAY];
	char *str = argv[0];

	if (strlen(str) >= countof(out)) {
		fprintf(stderr, "rv64_emu: to_upper takes at most %zu characters\n",
			countof(out) - 1);
		exit(EXIT_FAILURE);
	}

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
}

/* Test programs by the name that selects them on the command line */
static const struct test {
	const char *name;
	void (*run)(int argc, char **argv, struct rv_state *state);
} tests[] = {
	{ "fib_rec", fib_rec_test },
	{ "get_bitseq", get_bitseq_test },
	{ "int_to_str", int_to_str_test },
	{ "is_pal", pal_test },
	{ "max3", max3_test },
	{ "midpoint", midpoint_test },
	{ "quadratic", quadratic_test },
	{ "sort", sort_test },
	{ "str_to_int", str_to_int_test },
	{ "to_upper", to_upper_test },
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
 * return false if there is no such program.
 */
bool run_test(const char *name, int argc, char **argv,
	      struct rv_state *state)
{
	if (const struct test *test = find_test(name)) {
		test->run(argc, argv, state);
		return true;
	}
	return false;
}
