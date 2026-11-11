#!/bin/sh
# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Run rv64_emu once for every line of tests/cases and compare the combined
# output with tests/expected.  Each line holds the command line arguments of
# one run.  Set RUN to a command prefix, such as qemu-riscv64, to run the
# program under an emulator.  A run that takes more than TIMEOUT seconds,
# 30 by default, is killed if timeout(1) is available.  With -u, rewrite
# tests/expected instead.
#
# Usage: tests/run.sh [-u] <program>

# Split the arguments of each case into words without expanding globs
set -u -f

dir=$(dirname "$0")
update=false
if [ "${1-}" = -u ]; then
	update=true
	shift
fi
if [ $# -ne 1 ]; then
	echo "usage: $0 [-u] <program>" >&2
	exit 2
fi
prog=$1

actual=$(mktemp)
trap 'rm -f "$actual"' EXIT

# Do not leave core files of crashing runs behind
ulimit -c 0

# Send SIGKILL, since qemu-user may not react to SIGTERM in a tight loop
limit=
if command -v timeout >/dev/null; then
	limit="timeout -s KILL ${TIMEOUT-30}"
fi

while IFS= read -r args || [ -n "$args" ]; do
	printf '$ %s\n' "$args"
	# Split limit, RUN and the arguments into words
	$limit ${RUN-} "$prog" $args 2>&1
	printf '[exit %d]\n' $?
done <"$dir/cases" >"$actual"

if $update; then
	cp "$actual" "$dir/expected"
	exit 0
fi

if diff -u "$dir/expected" "$actual"; then
	echo "All $(grep -c '^\$ ' "$actual") cases passed"
else
	exit 1
fi
