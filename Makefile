# SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

CFLAGS = -std=c2y -g -Wall -Wextra
ASFLAGS = -g -march=rv64g
LDFLAGS =
RUN =

PROG = rv64_emu

# Test programs, each with a C version and an assembly version
TESTS = fib_rec find_max_index get_bitseq is_pal_rec max3 str_to_int \
	int_to_str midpoint quadratic sort to_upper

# Link the assembly versions first and next to each other.  The cache
# statistics depend on where they end up, so this keeps them independent
# of the size of the C code.
OBJS = $(TESTS:=_s.o) rv64_emu.o harness.o rv_emu.o exec.o alu.o mem.o \
       control.o analysis.o bits.o cache.o verbose.o $(TESTS:=_c.o)

# Find the sources under src and the test programs in progs
VPATH = src src/emu src/cache progs

# The compiler lists the headers of each object in a .d file next to it
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -Isrc -Iprogs -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

check: $(PROG)
	RUN='$(RUN)' tests/run.sh ./$(PROG)

clean:
	rm -rf $(PROG) $(OBJS) $(OBJS:.o=.d)

.PHONY: check clean

-include $(OBJS:.o=.d)
