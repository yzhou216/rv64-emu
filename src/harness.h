/* Runs of the test programs in C, in assembly, and in the emulator */

#ifndef HARNESS_H
#define HARNESS_H

#include "emu/rv_emu.h"

bool run_test(const char *name, int start_arg, int argc, char **argv,
	      struct rv_state *state);

#endif /* HARNESS_H */
