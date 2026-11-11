#!/bin/sh
# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Runs every test program through the command in the arguments, which runs
# rv64_emu, and fails unless the emulator gives each program the result
# that its assembly gives natively, with and without a cache.
#
# Usage: agree.sh <command>...

status=0
while read -r args; do
	# shellcheck disable=SC2086 # Each line splits into arguments
	out=$("$@" $args 2>&1 </dev/null)
	code=$?
	asm=$(printf '%s\n' "$out" | sed -n 's/^Asm://p')
	emu=$(printf '%s\n' "$out" | sed -n 's/^Emu://p')
	if [ "$code" -eq 0 ] && [ -n "$asm" ] && [ "$asm" = "$emu" ]; then
		echo "agree: $args"
	else
		echo "differ: $args (exit status $code)"
		printf '%s\n' "$out"
		status=1
	fi
done <<CASES
fib_rec 10
get_bitseq 94116 12 15
int_to_str 255 -o 2
is_pal racecar
max3 -5 -2 -9
midpoint 2 8
quadratic 2 3 4 5
sort 5 3 9 1 7
str_to_int 0x1f
to_upper hello
-a sort 5 3 9 1 7
-dm 64 4 to_upper hello
-sa 64 4 4 is_pal racecar
CASES
exit $status
