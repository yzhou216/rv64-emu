/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>
#include <stdbool.h>

uint64_t fib_rec_c(uint32_t);
uint64_t fib_rec_s(uint32_t);

int get_bitseq_c(int value, int start, int end);
int get_bitseq_s(int value, int start, int end);

bool pal_rec_c(char *str, int start, int end);
bool pal_rec_s(char *str, int start, int end);

int max3_c(int a, int b, int c);
int max3_s(int a, int b, int c);

void int_to_str_c(int value, char *out, int base);
void int_to_str_s(int value, char *out, int base);

int midpoint_c(int start, int end);
int midpoint_s(int start, int end);

int quadratic_c(int x, int a, int b, int c);
int quadratic_s(int x, int a, int b, int c);

void sort_c(int values[], int len);
void sort_s(int values[], int len);

int str_to_int_c(char *str, int base);
int str_to_int_s(char *str, int base);

void to_upper_c(char *src, char *dst);
void to_upper_s(char *src, char *dst);
