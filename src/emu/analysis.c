#include <stdio.h>

#include "analysis.h"

static void print_pct(char *fmt, int count, int total)
{
	double percent = 0.0;

	if (total)
		percent = (double)count / (double)total *100.0;
	printf(fmt, count, percent);
}

void rv_print(struct rv_analysis *analysis)
{
	int branches = analysis->taken + analysis->not_taken;

	printf("=== Analysis\n");
	print_pct("Instructions Executed  = %d\n", analysis->insns,
		  analysis->insns);
	print_pct("R-type + I-type        = %d (%.2f%%)\n", analysis->alu,
							    analysis->insns);
	print_pct("Loads                  = %d (%.2f%%)\n", analysis->loads,
							    analysis->insns);
	print_pct("Stores                 = %d (%.2f%%)\n", analysis->stores,
							    analysis->insns);
	print_pct("Jumps/JAL/JALR         = %d (%.2f%%)\n", analysis->jumps,
							    analysis->insns);
	print_pct("Conditional branches   = %d (%.2f%%)\n", branches,
							    analysis->insns);
	print_pct("  Branches taken       = %d (%.2f%%)\n", analysis->taken,
							    branches);
	print_pct("  Branches not taken   = %d (%.2f%%)\n", analysis->not_taken,
							    branches);
}
