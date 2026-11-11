# rv64_emu

64-bit RISC-V emulator with instruction cache support

Each test program comes in two versions, one in C and one in RISC-V
assembly.  `rv64_emu` runs the C version, calls the assembly version
natively, then runs that same assembly through the emulator and prints
all three results.  It can also count the instructions it executes and
simulate an instruction cache.

Since the emulator reads instructions straight out of its own binary,
it only runs on 64-bit RISC-V GNU/Linux, natively or under
`qemu-riscv64`.

## Building

On a RISC-V machine:

    make

## Running

    ./rv64_emu fib_rec 10
    ./rv64_emu -a sort 5 3 9 1 7
    ./rv64_emu -dm 64 4 to_upper hello
    ./rv64_emu -sa 64 4 4 is_pal racecar

`-a` prints the instruction mix, `-dm <size> <block size>` simulates a
direct-mapped cache, and `-sa <size> <block size> <ways>` simulates a
set-associative one.  The other programs are `get_bitseq`,
`int_to_str`, `max3`, `midpoint`, `quadratic`, and `str_to_int`.

The `c2y` branch reworks it in C2y and adds a test suite.
