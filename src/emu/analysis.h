/* Dynamic instruction mix, which -a prints */

#ifndef ANALYSIS_H
#define ANALYSIS_H

/* Executed instructions by kind */
struct rv_analysis {
	int insns;
	int alu;
	int loads;
	int stores;
	int jumps;
	int taken;
	int not_taken;
};

void rv_print(struct rv_analysis *analysis);

#endif /* ANALYSIS_H */
