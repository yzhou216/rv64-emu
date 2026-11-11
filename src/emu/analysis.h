/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* Dynamic instruction mix, which -a prints */

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <stdint.h>

/* Executed instructions by kind */
struct rv_analysis {
	uint64_t insns;
	uint64_t alu;
	uint64_t loads;
	uint64_t stores;
	uint64_t jumps;
	uint64_t taken;
	uint64_t not_taken;
};

void rv_print(const struct rv_analysis *analysis);

#endif /* ANALYSIS_H */
