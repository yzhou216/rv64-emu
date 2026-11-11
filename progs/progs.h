/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* C and assembly versions of the test programs */

#ifndef PROGS_H
#define PROGS_H

#include <stdint.h>

int fib_rec_c(int n);
uint64_t fib_rec_s(uint32_t);

uint32_t get_bitseq_c(uint32_t value, int start, int end);
int get_bitseq_s(int value, int start, int end);

bool pal_rec_c(const char *str, int start, int end);
bool pal_rec_s(const char *str, int start, int end);

int max3_c(int a, int b, int c);
int max3_s(int a, int b, int c);

void int_to_str_c(uint32_t value, char *out, int base);
void int_to_str_s(uint32_t value, char *out, int base);

int midpoint_c(int start, int end);
int midpoint_s(int start, int end);

int quadratic_c(int x, int a, int b, int c);
int quadratic_s(int x, int a, int b, int c);

int find_max_index_c(const int *values, int len);
void sort_c(int values[], int len);
void sort_s(int values[], int len);

int str_to_int_c(const char *str, int base);
int str_to_int_s(const char *str, int base);

int insn_s(uint64_t *results);

void to_upper_c(const char *src, char *dst);
void to_upper_s(const char *src, char *dst);

#endif /* PROGS_H */
