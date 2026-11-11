/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "progs.h"

#define MAX_ARRAY 1024

void copy_array(int *dst, int *src, int len)
{
	for (int i = 0; i < len; i++)
		dst[i] = src[i];
}

void print_array(char *prefix, int *values, int len)
{
	printf("%s", prefix);
	for (int i = 0; i < len; i++)
		printf(" %d", values[i]);
	printf("\n");
}

void fib_rec_test(int start_arg, char **argv, struct rv_state *state)
{
	int result;
	int n = atoi(argv[start_arg]);

	result = fib_rec_c(n);
	printf("C: %d\n", result);

	result = fib_rec_s(n);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)fib_rec_s, n, 0, 0, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
}

void get_bitseq_test(int start_arg, char **argv, struct rv_state *state)
{
	int result;
	int n = atoi(argv[start_arg]);
	int start = atoi(argv[start_arg + 1]);
	int end = atoi(argv[start_arg + 2]);

	result = get_bitseq_c(n, start, end);
	printf("C: %d\n", result);

	result = get_bitseq_s(n, start, end);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)get_bitseq_s, n, start, end, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
}

void pal_test(int start_arg, char **argv, struct rv_state *state)
{
	bool result;
	char *str = argv[start_arg];
	int end = strlen(str) - 1;

	result = pal_rec_c(str, 0, end);
	printf("C: %s\n", result ? "is a palindrome" : "is not a palindrome");

	result = pal_rec_s(str, 0, end);
	printf("Asm: %s\n", result ? "is a palindrome" : "is not a palindrome");

	rv_init(state, (uint32_t *)pal_rec_s, (uint64_t)str, 0, end, 0);
	result = rv_emulate(state);
	printf("Emu: %s\n", result ? "is a palindrome" : "is not a palindrome");
}

void int_to_str_test(int start_arg, char **argv, struct rv_state *state)
{
	char out[MAX_ARRAY];

	int value = atoi(argv[start_arg]);
	int base = 10;
	if (!strncmp(argv[start_arg + 1], "-o", 2))
		base = atoi(argv[start_arg + 2]);

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

void max3_test(int start_arg, char **argv, struct rv_state *state)
{
	int result;
	int a = atoi(argv[start_arg]);
	int b = atoi(argv[start_arg + 1]);
	int c = atoi(argv[start_arg + 2]);

	result = max3_c(a, b, c);
	printf("C: %d\n", result);

	result = max3_s(a, b, c);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)max3_s, a, b, c, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
}

void midpoint_test(int start_arg, char **argv, struct rv_state *state)
{
	int result;
	int start = atoi(argv[start_arg]);
	int end = atoi(argv[start_arg + 1]);

	result = midpoint_c(start, end);
	printf("C: %d\n", result);

	result = midpoint_s(start, end);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)midpoint_s, start, end, 0, 0);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
}

void quadratic_test(int start_arg, char **argv, struct rv_state *state)
{
	int result;
	int x = atoi(argv[start_arg]);
	int a = atoi(argv[start_arg + 1]);
	int b = atoi(argv[start_arg + 2]);
	int c = atoi(argv[start_arg + 3]);

	result = quadratic_c(x, a, b, c);
	printf("C: %d\n", result);

	result = quadratic_s(x, a, b, c);
	printf("Asm: %d\n", result);

	rv_init(state, (uint32_t *)quadratic_s, x, a, b, c);
	result = rv_emulate(state);
	printf("Emu: %d\n", result);
}

void sort_test(int start_arg, int argc, char **argv, struct rv_state *state)
{
	int sorted[MAX_ARRAY];
	int values[MAX_ARRAY] = { 0, };
	int len = 0;

	for (int i = 0; i < argc - start_arg; i++) {
		values[i] = atoi(argv[i + start_arg]);
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

void str_to_int_test(int start_arg, int argc, char **argv, struct rv_state *state)
{
	char *str = argv[start_arg];
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

void to_upper_test(int start_arg, char **argv, struct rv_state *state)
{
	/* Make a copy of the string since we modify it in place */
	char out[MAX_ARRAY];
	char *str = argv[start_arg];

	strncpy(out, str, MAX_ARRAY);
	to_upper_c(str, out);
	printf("C: %s\n", out);

	strncpy(out, str, MAX_ARRAY);
	to_upper_s(str, out);
	printf("Asm: %s\n", out);

	strncpy(out, str, MAX_ARRAY);
	rv_init(state, (uint32_t *)to_upper_s, (uint64_t)str, (uint64_t)out,
		0, 0);
	rv_emulate(state);
	printf("Emu: %s\n", out);
}

/*
 * Run the test program called name on the arguments in argv from
 * start_arg on, and return false if there is no such program.
 */
bool run_test(const char *name, int start_arg, int argc, char **argv,
	      struct rv_state *state)
{
	if (!strcmp(name, "fib_rec"))
		fib_rec_test(start_arg, argv, state);
	else if (!strcmp(name, "get_bitseq"))
		get_bitseq_test(start_arg, argv, state);
	else if (!strcmp(name, "int_to_str"))
		int_to_str_test(start_arg, argv, state);
	else if (!strcmp(name, "is_pal"))
		pal_test(start_arg, argv, state);
	else if (!strcmp(name, "max3"))
		max3_test(start_arg, argv, state);
	else if (!strcmp(name, "midpoint"))
		midpoint_test(start_arg, argv, state);
	else if (!strcmp(name, "quadratic"))
		quadratic_test(start_arg, argv, state);
	else if (!strcmp(name, "sort"))
		sort_test(start_arg, argc, argv, state);
	else if (!strcmp(name, "str_to_int"))
		str_to_int_test(start_arg, argc, argv, state);
	else if (!strcmp(name, "to_upper"))
		to_upper_test(start_arg, argv, state);
	else
		return false;
	return true;
}
