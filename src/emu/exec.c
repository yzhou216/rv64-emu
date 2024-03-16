#include <stdio.h>
#include <stdlib.h>

#include "exec.h"
#include "bits.h"

static void unsupported(char *what, uint32_t value)
{
	printf("unsupported %s 0x%x\n", what, value);
	exit(-1);
}

/* Execute word, leaving pc at the instruction to run next */
void rv_exec(struct rv_state *state, uint32_t word)
{
	uint32_t opcode = get_bits(word, 0, 7);

	switch (opcode) {
	default:
		unsupported("Unknown opcode: ", opcode);
	}
}
