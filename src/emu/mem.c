#include <stdint.h>

#include "exec.h"
#include "bits.h"

void emu_load(struct rv_state *state, uint32_t word)
{
	uint32_t rs1 = get_bits(word, 15, 5);
	uint32_t rd = get_bits(word, 7, 5);
	uint32_t funct3 = get_bits(word, 12, 3);
	uint32_t imm = get_bits(word, 20, 12);
	int64_t offset = sign_extend(imm, 12);
	uint64_t addr = (uint64_t)((uint8_t *)state->regs[rs1] + offset);
	switch (funct3) {
	case 0b000: /* LB */
		state->regs[rd] = *((int8_t *)addr);
		break;
	case 0b011: /* LD */
		state->regs[rd] = *((uint64_t *)addr);
		break;
	default:
		unsupported("I-type funct3 (load)", funct3);
	}
	state->pc += 4; /* Next instruction */
}

void emu_s_type(struct rv_state *state, uint32_t word)
{
	uint32_t imm4_0 = get_bits(word, 7, 5);
	uint32_t imm11_5 = get_bits(word, 25, 7);
	uint32_t imm = imm4_0 | (imm11_5 << 5);
	uint32_t rs1 = get_bits(word, 15, 5);
	uint32_t rs2 = get_bits(word, 20, 5);
	uint32_t funct3 = get_bits(word, 12, 3);
	int64_t offset = sign_extend(imm, 12);
	uint64_t addr = (uint64_t)((uint8_t *)state->regs[rs1] + offset);
	switch (funct3) {
	case 0b000: /* SB */
		*((uint8_t *)addr) = state->regs[rs2];
		break;
	case 0b011: /* SD */
		*((uint64_t *)addr) = state->regs[rs2];
		break;
	default:
		unsupported("S-type funct3", funct3);
	}
	state->pc += 4;
}
