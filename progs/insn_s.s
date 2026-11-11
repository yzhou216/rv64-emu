# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

.align 4
.global insn_s

/*
 * insn_s exercises corner cases of the supported instructions that the
 * other test programs do not reach.  It stores one 64-bit result per
 * case in the array at a0 and returns the number of results.
 */
insn_s:
	mv a1, a0                   # a1 = next result

	# Loads and stores with negative offsets
	addi sp, sp, -16
	addi t0, sp, 16             # t0 = sp before the allocation
	li t1, -1234
	sd t1, -8(t0)
	ld t2, -8(t0)
	sd t2, 0(a1)
	li t1, -77
	sw t1, -12(t0)
	lw t2, -12(t0)
	sd t2, 8(a1)
	li t1, -5
	sb t1, -16(t0)
	lb t2, -16(t0)
	sd t2, 16(a1)
	ld t2, 8(sp)                # same doubleword as -8(t0)
	sd t2, 24(a1)
	addi sp, sp, 16
	addi a1, a1, 32

	# jalr with an odd offset, whose low bit the jump clears, linking
	# into a register other than ra
	jal t0, 1f                  # t0 = address of 1f
1:	jalr t1, 13(t0)             # jump 12 bytes past 1f
	li t2, -1
	j 2f
	sub t2, t1, t0              # t1 = 1f + 4
2:	sd t2, 0(a1)

	# jalr with rd equal to rs1
	jal t3, 3f                  # t3 = address of 3f
3:	mv t4, t3
	jalr t3, 16(t3)             # jump 16 bytes past 3f
	li t2, -1
	j 4f
	sub t2, t3, t4              # t3 = 3f + 8
4:	sd t2, 8(a1)
	addi a1, a1, 16

	# Shifts by 32 or more, which take six bits of the shift amount
	li t0, 3
	slli t2, t0, 40
	sd t2, 0(a1)
	srli t2, t2, 39
	sd t2, 8(a1)
	li t1, 66                   # sll and srl use only the low six bits
	sll t2, t0, t1
	sd t2, 16(a1)
	srl t2, t2, t1
	sd t2, 24(a1)
	addi a1, a1, 32

	# blt and bge compare signed values, so -1 is less than 1, and each
	# branch that does not jump adds its bit to the result
	li t0, -1
	li t1, 1
	li t2, 0
	blt t0, t1, 5f              # jumps
	addi t2, t2, 1
5:	bge t1, t0, 6f              # jumps
	addi t2, t2, 2
6:	blt t1, t0, 7f              # falls through
	addi t2, t2, 4
7:	bge t0, t1, 8f              # falls through
	addi t2, t2, 8
8:	sd t2, 0(a1)                # 12
	addi a1, a1, 8

	# Division by zero and the most negative value divided by -1, whose
	# results RISC-V defines, and rounding toward zero
	li t0, 7
	div t2, t0, zero            # -1
	sd t2, 0(a1)
	rem t2, t0, zero            # 7
	sd t2, 8(a1)
	li t0, 1
	slli t0, t0, 63             # most negative value
	li t1, -1
	div t2, t0, t1              # most negative value
	sd t2, 16(a1)
	rem t2, t0, t1              # 0
	sd t2, 24(a1)
	li t0, -7
	li t1, 2
	div t2, t0, t1              # -3
	sd t2, 32(a1)
	rem t2, t0, t1              # -1, with the sign of the dividend
	sd t2, 40(a1)
	addi a1, a1, 48

	# Writes to x0, which must stay zero, so these cases come last
	addi zero, zero, 5
	add t2, zero, zero
	sd t2, 0(a1)
	li t1, 7
	sd t1, 8(a1)
	ld zero, 8(a1)
	add t2, zero, zero
	sd t2, 8(a1)
	addi a1, a1, 16

	sub a0, a1, a0              # a0 = number of results
	srli a0, a0, 3
	ret
