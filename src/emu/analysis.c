/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>

#include "analysis.h"

static void print_pct(const char *fmt, uint64_t count, uint64_t total)
{
	double percent = 0.0;

	if (total)
		percent = (double)count / (double)total * 100.0;
	printf(fmt, count, percent);
}

void rv_print(const struct rv_analysis *analysis)
{
	uint64_t branches = analysis->taken + analysis->not_taken;

	printf("=== Analysis\n");
	print_pct("Instructions Executed  = %w64u\n", analysis->insns,
		  analysis->insns);
	print_pct("R-type + I-type        = %w64u (%.2f%%)\n", analysis->alu,
		  analysis->insns);
	print_pct("Loads                  = %w64u (%.2f%%)\n", analysis->loads,
		  analysis->insns);
	print_pct("Stores                 = %w64u (%.2f%%)\n", analysis->stores,
		  analysis->insns);
	print_pct("Jumps/JAL/JALR         = %w64u (%.2f%%)\n", analysis->jumps,
		  analysis->insns);
	print_pct("Conditional branches   = %w64u (%.2f%%)\n", branches,
		  analysis->insns);
	print_pct("  Branches taken       = %w64u (%.2f%%)\n", analysis->taken,
		  branches);
	print_pct("  Branches not taken   = %w64u (%.2f%%)\n",
		  analysis->not_taken, branches);
}
