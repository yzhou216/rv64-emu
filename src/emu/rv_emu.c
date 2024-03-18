#include <stdio.h>
#include <string.h>

#include "exec.h"

#define DEBUG 0

/* Fetch and execute the instruction at pc */
static void rv_one(struct rv_state *state)
{
	uint32_t word = *((uint32_t *)state->pc);
	//word = cache_lookup(&state->icache, (uint64_t) state->pc);

#if DEBUG
	printf("iw: %x\n", word);
#endif

	rv_exec(state, word);
}

void rv_init(struct rv_state *state, uint32_t *target,
	     uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3)
{
	state->pc = (uint64_t) target;
	state->regs[RV_A0] = a0;
	state->regs[RV_A1] = a1;
	state->regs[RV_A2] = a2;
	state->regs[RV_A3] = a3;

	state->regs[RV_ZERO] = 0;	// zero is always 0  (:
	state->regs[RV_RA] = RV_STOP;
	state->regs[RV_SP] = (uint64_t) & state->stack[STACK_SIZE];

	memset(&state->analysis, 0, sizeof(struct rv_analysis));
	cache_init(&state->icache);
}

uint64_t rv_emulate(struct rv_state *state)
{
	while (state->pc != RV_STOP) {
		rv_one(state);
		state->analysis.insns++;
	}
	return state->regs[RV_A0];
}
