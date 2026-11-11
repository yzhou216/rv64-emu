<!--
SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>

SPDX-License-Identifier: GPL-3.0-or-later
-->

# rv64_emu

64-bit RISC-V emulator with instruction cache support

Each test program comes in two versions, one in C and one in RISC-V
assembly.  `rv64_emu` runs the C version, calls the assembly version
natively, then runs that same assembly through the emulator and prints
all three results.  It can also count the instructions it executes and
simulate an instruction cache.  `insn` only has an assembly version and
checks instruction corner cases.

Since the emulator reads instructions straight out of its own binary,
it only runs on 64-bit RISC-V GNU/Linux, natively or under
`qemu-riscv64`.

## Building

The code uses C2y features, so it needs a recent toolchain, such as
GCC 16 and glibc 2.39.  On a RISC-V machine:

    make
    make check

To cross-compile, point `CC` and `AS` at a RISC-V toolchain and set
`RUN` to run the tests under QEMU:

    make check CC=riscv64-linux-gnu-gcc AS=riscv64-linux-gnu-as \
        RUN='qemu-riscv64 -L /usr/riscv64-linux-gnu'

With Nix, 64-bit x86 and ARM GNU/Linux systems cross-build it as well,
and `nix flake check` runs the tests under QEMU:

    nix build
    nix flake check

`nix develop` opens a shell in which `make` uses the same toolchain and
`make check RUN=qemu-riscv64` runs the tests.

## Running

    ./rv64_emu fib_rec 10
    ./rv64_emu -a sort 5 3 9 1 7
    ./rv64_emu -dm 64 4 to_upper hello
    ./rv64_emu -sa 64 4 4 is_pal racecar

Run `./rv64_emu` without arguments to list the options and programs.

## License

rv64_emu is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
