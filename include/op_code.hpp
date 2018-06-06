#pragma once

#include <cstdint>

namespace gb_emu
{
	/**
	 * GB Opcodes deconstructed into sub-byte components
	 */
	enum class Opcode_Group : uint8_t {
		MISC1 = 0x00,
		LD = 0x40,
		ARITH = 0x80,
		MISC2 = 0xC0,
		MASK = 0xC0,
	};

	constexpr Opcode_Group operator & (Opcode_Group lhs, Opcode_Group rhs) {
		return static_cast<Opcode_Group>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
	}

	enum class Opcode : uint8_t {
		// HALT (p.97)
		// Power down CPU unitl interrupt occurs (saves power). 4 cycles
		HALT = 0x76,
	};

	enum class Opcode_Register : uint8_t {
		A = 0x7,
		B = 0x0,
		C = 0x1,
		D = 0x2,
		E = 0x3,
		H = 0x4,
		L = 0x5,
		HL = 0x6,
	};

	enum class Opcode_Register_Pair : uint8_t {
		BC = 0x0,
		DE = 0x1,
		HL = 0x2,
		SP = 0x3,
	};

	enum class Opcode_Register_Pair_Address : uint8_t {
		BC = 0x0,
		DE = 0x1,
		HL_plus = 0x2,
		HL_minus = 0x3,
	};

	enum class Opcode_Arithmetic_Command : uint8_t {
		ADD = 0,
		ADC = 1,
		SUB = 2,
		SBC = 3,
		AND = 4,
		XOR = 5,
		OR = 6,
		CP = 7,
	};

	enum class Opcode_Misc1_Command_Groups : uint8_t {
		MISC1 = 0x0,
		LD_r1_d16 = 0x1,
		LD_add_A = 0x2,
		INC_rr = 0x3,
		INC_r_1 = 0x4,
		DEC_r_1 = 0x5,
		LD_r1_d8_1 = 0x6,
		MISC2 = 0x7,
		MISC3 = 0x8,
		ADD_HL_rr1 = 0x9,
		LD_A_add = 0xA,
		DEC_rr = 0xB,
		INC_r_2 = 0xC,
		DEC_r_2 = 0xD,
		LD_r1_d8_2 = 0xE,
		MISC4 = 0xF,
	};

	enum class Opcode_Misc2_Command_Groups : uint8_t {
		POP = 0x1,
		PUSH = 0x5,
		ARITH_1 = 0x6,
		RST_1 = 0x7,
		ARITH_2 = 0xE,
		RST_2 = 0xF,
	};

	enum class Opcode_Prefix_Group : uint8_t {
		MISC1 = 0x00,
		TEST_BIT = 0x40,
		CLEAR_BIT = 0x80,
		SET_BIT = 0xC0,
		MASK = 0xC0,
	};

	constexpr Opcode_Prefix_Group operator & (Opcode_Prefix_Group lhs, Opcode_Prefix_Group rhs) {
		return static_cast<Opcode_Prefix_Group>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
	}

	enum class Opcode_Prefix_Misc1_Command_Groups : uint8_t {
		ROTATE = 0x0,
		ROTATE_THRU_CARRY = 0x10,
		SHIFT = 0x20,
		SWAP_SHIFT = 0x30,
		MASK = 0xF0,
	};

	constexpr Opcode_Prefix_Misc1_Command_Groups operator & (Opcode_Prefix_Misc1_Command_Groups lhs, Opcode_Prefix_Misc1_Command_Groups rhs) {
		return static_cast<Opcode_Prefix_Misc1_Command_Groups>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
	}

	/*struct LD_Op {
		Opcode op;
		Register r1, r2;
		LD_Op(uint8_t opcode) : op{opcode & static_cast<uint8_t>(Opcode_Group::MASK)}, r1(opcode & 0x38), r2(opcode & 0x07) {}
	};*/

	/**
	 * GB Opcodes as provided by the GP CPU Manual from p.65
	 * onwards.
	 */
	enum class Opcode_Exact : uint8_t {
		// NOP (p.97)
		// 4 cycles
		NOP = 0x00,

		// LD nn, n (p.65)
		// All are 8 cycles
		LD_B_n = 0x06,
		LD_C_n = 0x0E,
		LD_D_n = 0x16,
		LD_E_n = 0x1E,
		LD_H_n = 0x26,
		LD_L_n = 0x2E, /** n=8 bit immediate value */

		// LD r1, r2, (p.66)
		// All are 4 cycles xHL instructions (marked)
		LD_A_A = 0x7F,
		LD_A_B = 0x78,
		LD_A_C = 0x79,
		LD_A_D = 0x7A,
		LD_A_E = 0x7B,
		LD_A_H = 0x7C,
		LD_A_L = 0x7D,
		LD_A_HL = 0x7E, /** 8 cycles */
		LD_B_B = 0x40,
		LD_B_C = 0x41,
		LD_B_D = 0x42,
		LD_B_E = 0x43,
		LD_B_H = 0x44,
		LD_B_L = 0x45,
		LD_B_HL = 0x46, /** 8 cycles */
		LD_C_B = 0x48,
		LD_C_C = 0x49,
		LD_C_D = 0x4A,
		LD_C_E = 0x4B,
		LD_C_H = 0x4C,
		LD_C_L = 0x4D,
		LD_C_HL = 0x4E, /** 8 cycles */
		LD_D_B = 0x50,
		LD_D_C = 0x51,
		LD_D_D = 0x52,
		LD_D_E = 0x53,
		LD_D_H = 0x54,
		LD_D_L = 0x55,
		LD_D_HL = 0x56, /** 8 cycles */
		LD_E_B = 0x58,
		LD_E_C = 0x59,
		LD_E_D = 0x5A,
		LD_E_E = 0x5B,
		LD_E_H = 0x5C,
		LD_E_L = 0x5D,
		LD_E_HL = 0x5E, /** 8 cycles */
		LD_H_B = 0x60,
		LD_H_C = 0x61,
		LD_H_D = 0x62,
		LD_H_E = 0x63,
		LD_H_H = 0x64,
		LD_H_L = 0x65,
		LD_H_HL = 0x66, /** 8 cycles */
		LD_L_B = 0x68,
		LD_L_C = 0x69,
		LD_L_D = 0x6A,
		LD_L_E = 0x6B,
		LD_L_H = 0x6C,
		LD_L_L = 0x6D,
		LD_L_HL = 0x6E, /** 8 cycles */
		LD_HL_B = 0x70, /** 8 cycles */
		LD_HL_C = 0x71, /** 8 cycles */
		LD_HL_D = 0x72, /** 8 cycles */
		LD_HL_E = 0x73, /** 8 cycles */
		LD_HL_H = 0x74, /** 8 cycles */
		LD_HL_L = 0x75, /** 8 cycles */
		LD_HL_n = 0x36, /** 12 cycles */

		// LD A,n (p.68)
		// Single register instructions are 4 cycles
		// Paired register instructions are 8 cycles
		//LD_A_A = 0x7F, /** 4 cycles vvv */
		//LD_A_B = 0x78,
		//LD_A_C = 0x79,
		//LD_A_D = 0x7A,
		//LD_A_E = 0x7B,
		//LD_A_H = 0x7C,
		//LD_A_L = 0x7D,
		LD_A_BC = 0x0A, /** 8 cycles vvv */
		LD_A_DE = 0x1A,
		//LD_A_HL = 0x7E,
		LD_A_n = 0x3E, /** n=8 bit immediate value */
		LD_A_nn = 0xFA, /** 16 cycles. nn=16 bit immediate value (LS byte first) */

		// LD n,A (p.69)
		LD_B_A = 0x47, /** 4 cycles vvv */
		LD_C_A = 0x4F,
		LD_D_A = 0x57,
		LD_E_A = 0x5F,
		LD_H_A = 0x67,
		LD_L_A = 0x6F,
		LD_BC_A = 0x02, /** 8 cycles vvv */
		LD_DE_A = 0x12,
		LD_HL_A = 0x77,
		LD_nn_A = 0xEA, /** 16 cycles, nn=16 bit imm. value (LS byte first) */

		// LD A,(C) (p.70)
		// 8 cycles
		// Put value at address $FF00 + register C into A
		// i.e LD A,($FF00+C)
		LD_A_offsetC = 0xF2,

		// LD (C),A (p.70)
		// 8 cycles, as above
		LD_offsetC_A = 0xE2,

		// LDD A,(HL) (p.71)
		// Same as LD A, (HLD) and LD A,(HL-)
		// Put value at address HL into A, decrement HL
		// Same as LD A, (HL) - DEC HL
		// 8 cycles
		LDD_A_HL = 0x3A,

		// LDD (HL),A (p.72)
		// Same as LDD (HLD), A and LDD (HL-), A
		// Put A into address HL, decrement HL
		// Same as LD (HL), A - DEC HL
		LDD_HL_A = 0x32,

		// LDI A,(HL) (p.73)
		// Increment version of LDD_AHL
		LDI_A_HL = 0x2A,

		// LDI (HL),A (p.74)
		// Increment version of LDD_HLA
		LDI_HL_A = 0x22,

		// LDH (n),A - LDH A,(n) (p.75)
		// A and memory address $FF00+n, where n is 1 byte imm.
		// 12 cycles
		LDH_n_A = 0xE0,
		LDH_A_n = 0xF0,

		// LD n,nn (p.76)
		// 2 byte imm. loads
		// 12 cycles
		// SP = Stack Pointer
		LD_BC_nn = 0x01,
		LD_DE_nn = 0x11,
		LD_HL_nn = 0x21,
		LD_SP_nn = 0x31,

		// LD SP, HL (p.76), 8 cycles
		LD_SP_HL = 0xF9,

		// LDHL SP,n (p.77)
		// Load (SP+n) effective address into HL
		// n = 1 byte imm.
		// Resets Z,N flags, H,C set/reset according to operation
		// 12 cycles
		LDHL_SP_n = 0xF8,

		// LD (nn),SP (p.78)
		// Put SP at nn (2-byte imm. address)
		// 20 cycles
		LD_nn_SP = 0x08,

		// PUSH nn (p.78)
		// Push register pair nn onto stack
		// Decrement SP twice
		// 16 cycles
		PUSH_AF = 0xF5,
		PUSH_BC = 0xC5,
		PUSH_DE = 0xD5,
		PUSH_HL = 0xE5,

		// POP nn (p.79)
		// Pop two bytes off stack into register pair
		// Increment SP twice
		// 12 cycles
		POP_AF = 0xF1,
		POP_BC = 0xC1,
		POP_DE = 0xD1,
		POP_HL = 0xE1,

		// ADD A,n (p.80)
		// Add n to A
		// 4 cycles except as marked
		// Flags: Z set if result is 0. N reset. H set if carry from bit 3.
		// C if carry from bit 7
		ADD_A_A = 0x87,
		ADD_A_B = 0x80,
		ADD_A_C = 0x81,
		ADD_A_D = 0x82,
		ADD_A_E = 0x83,
		ADD_A_H = 0x84,
		ADD_A_L = 0x85,
		ADD_A_HL = 0x86, /** 8 cycles */
		ADD_A_n = 0xC6,  /** 8 cycles, 1 byte imm. value */

		// ADC A,n (p.81)
		// Add n + carry flag to A
		// Flags as in ADD A,n
		// 4 cycles except as marked
		ADC_A_A = 0x8F,
		ADC_A_B = 0x88,
		ADC_A_C = 0x89,
		ADC_A_D = 0x8A,
		ADC_A_E = 0x8B,
		ADC_A_H = 0x8C,
		ADC_A_L = 0x8D,
		ADC_A_HL = 0x8E, /** 8 cycles */
		ADC_A_n = 0xCE, /** 8 cycles, 1 byte imm. value */

		// SUB n (p.82)
		// Subtract n from A
		// 4 cycles except as marked
		// Flags: Z set if result is 0. N set. H set if no borrow from bit 4. C set if no borrow
		SUB_A = 0x97,
		SUB_B = 0x90,
		SUB_C = 0x91,
		SUB_D = 0x92,
		SUB_E = 0x93,
		SUB_H = 0x94,
		SUB_L = 0x95,
		SUB_HL = 0x96, /** 8 cycles */
		SUB_n = 0xD6,  /** 8 cycles, 1 byte imm. value */

		// SBC A,n (p.83)
		// Subtract n + carry flag from A
		// Flags as in SUB n
		// 4 cycles except as marked
		SBC_A_A = 0x9F,
		SBC_A_B = 0x98,
		SBC_A_C = 0x99,
		SBC_A_D = 0x9A,
		SBC_A_E = 0x9B,
		SBC_A_H = 0x9C,
		SBC_A_L = 0x9D,
		SBC_A_HL = 0x9E, /** 8 cycles */
		SBC_A_n = 0xDE, /** Marked as Opcode = ??, cycles = ?? */

		// AND n (p.84)
		// Logical AND n with A, result in A
		// 4 cycles except as marked
		// Flags: Z set if result is 0. N, C reset. H set
		AND_A = 0xA7,
		AND_B = 0xA0,
		AND_C = 0xA1,
		AND_D = 0xA2,
		AND_E = 0xA3,
		AND_H = 0xA4,
		AND_L = 0xA5,
		AND_HL = 0xA6, /** 8 cycles */
		AND_n = 0xE6, /** 8 cycles */

		// OR n (p.85)
		// Logical OR n with A, result in A
		// 4 cycles except as marked
		// Flags: Z set if result is 0. N, H, C reset.
		OR_A = 0xB7,
		OR_B = 0xB0,
		OR_C = 0xB1,
		OR_D = 0xB2,
		OR_E = 0xB3,
		OR_H = 0xB4,
		OR_L = 0xB5,
		OR_HL = 0xB6, /** 8 cycles */
		OR_n = 0xF6, /** 8 cycles */

		// XOR n (p.86)
		// Logical XOR n with A, result in A
		// 4 cycles except as marked
		// Flags: Z set if result is 0. N, H, C reset.
		XOR_A = 0xAF,
		XOR_B = 0xA8,
		XOR_C = 0xA9,
		XOR_D = 0xAA,
		XOR_E = 0xAB,
		XOR_H = 0xAC,
		XOR_L = 0xAD,
		XOR_HL = 0xAE, /** 8 cycles */
		XOR_n = 0xEE, /** 8 cycles */

		// CP n (p.87)
		// Compare A with n.
		// 4 cycles except as markd
		// Flags: Z set if A=n. N set. H set if no borrow from bit 4. C set for no borrow (A < n)
		CP_A = 0xBF,
		CP_B = 0xB8,
		CP_C = 0xB9,
		CP_D = 0xBA,
		CP_E = 0xBB,
		CP_H = 0xBC,
		CP_L = 0xBD,
		CP_HL = 0xBE, /** 8 cycles */
		CP_n = 0xFE, /** 8 cycles */

		// INC n (p.88)
		// Increment register
		// 4 cycles except as marked
		// Flags: Z set if result is 0. N reset. H set if carry from bit 3. C not affected
		INC_A = 0x3C,
		INC_B = 0x04,
		INC_C = 0x0C,
		INC_D = 0x14,
		INC_E = 0x1C,
		INC_H = 0x24,
		INC_L = 0x2C,
		INC_addressHL = 0x34, /** 12 cycles */

		// DEC n (p.89)
		// Decrement register
		// 4 cycles except as marked
		// Flags: Z set if result is 0. N set. H set if carry from bit 4. C not affected
		DEC_A = 0x3D,
		DEC_B = 0x05,
		DEC_C = 0x0D,
		DEC_D = 0x15,
		DEC_E = 0x1D,
		DEC_H = 0x25,
		DEC_L = 0x2D,
		DEC_addressHL = 0x35, /** 12 cycles */

		// ADD HL,n (p.90)
		// Add n to HL (2 byte arithmetic)
		// 8 cycles
		// Flags: Z not affected. N reset. H set if carry from bit 11. C set if carry from bit 15
		ADD_HL_BC = 0x09,
		ADD_HL_DE = 0x19,
		ADL_HL_HL = 0x29,
		ADD_HL_SP = 0x39,

		// ADD SP,n (p.91)
		// Add n (1 byte imm. value) to SP
		// 16 cycles
		// Flags: Z, N reset. H,C set or reset according to operation
		ADD_SP_n = 0xE8,

		// INC nn (p.92)
		// Increment register. 8 cycles
		INC_BC = 0x03,
		INC_DE = 0x13,
		INC_HL = 0x23,
		INC_SP = 0x33,

		// DEC nn (p.93)
		// Decrement register. 8 cycles
		DEC_BC = 0x0B,
		DEC_DE = 0x1B,
		DEC_HL = 0x2B,
		DEC_SP = 0x3B,

		// CB prefix. Next byte gives another instruction
		// See CB_Opcode below
		CB = 0xCB,

		// DAA (p.95)
		// Decimal adjust register A. 4 cycles
		// Flags: Z set if A is zero. N not affect. H reset. C set or reset according to operation
		DAA = 0x27,

		// CPL (p.95)
		// Complement A (Not A, flip all bits). 4 cycles
		// Flags: Z, C not affected. N, H set
		CPL = 0x2F,

		// CCF (p. 96)
		// Complement carry flag. 4 cycles
		// Flags: Z not affected. N, H reset. C flipped
		CCF = 0x3F,

		// SCF (p.96)
		// Set carry flag. 4 cycles
		// Flags: Z not affected, N, H reset. C set
		SCF = 0x37,

		// HALT (p.97)
		// Power down CPU unitl interrupt occurs (saves power). 4 cycles
		HALT = 0x76,

		// STOP (p.97)
		// Halt CPU & LCD until button pressed. 4 cycles
		// Always followed by a 0x00
		STOP = 0x10,

		// DI (p.98)
		// Disable interrupts after this instruction. 4 cycles
		DI = 0xF3,

		// EI (p.98)
		// Enable interrupts after this instruction. 4 cycles
		EI = 0xFB,

		// RLCA (p.99)
		// Rotate A left. Old bit 7 to Carry Flag. 4 cycles
		// Flags: Z set if result is 0. N, H reset. C contains old bit 7
		RLCA = 0x07,

		// RLA (p.99)
		// Rotate A left through Carry Flag. 4 cycles
		// Flags: Z set if result is 0. N, H reset. C contains old bit 7
		RLA = 0x17,

		// RRCA (p.99)
		// Rotate A right. Old bit 0 to Carry Flag. 4 cycles
		// Flags: Z set if result is 0. N, H reset. C contains old bit 0
		RRCA = 0x0F,

		// RRA (p.99)
		// Rotate A right through Carry Flag. 4 cycles
		// Flags: Z set if result is 0. N, H reset. C contains old bit 0
		RRA = 0x1F,

		// JP nn (p.111)
			// Jump to address. 12 cycles
			// nn = 2 byte imm. value (LS byte first)
			JP_nn = 0xC3,

			// JP cc,nn (p.111)
			// Jump to address n if:
			// cc==NZ, jump if Z flag is reset
			// cc==Z, jump if Z flag is set
			// cc==NC, jump if C flag is reset
			// cc==C, jump if C flag is set
			// 12 cycles
			// nn = 2 byte imm. value (LS byte first)
			JP_NZ_nn = 0xC2,
			JP_Z_nn = 0xCA,
			JP_NC_nn = 0xD2,
			JP_C_nn = 0xDA,

			// JP (HL) (p.112)
			// Jump to address in HL. 4 cycles
			JP_HL = 0xE9,

			// JR n (p.112)
			// Add n to current address and jump to it
			// n = 1 byte imm. value. 8 cycles
			JR_n = 0x18,

			// JR cc,n (p.113)
			// If condition (see JP cc,nn), then do JR n
			// 8 cycles
			JR_NZ_n = 0x20,
			JR_Z_n = 0x28,
			JR_NC_n = 0x30,
			JR_C_n = 0x38,

			// CALL nn (p.114)
			// Push address of next instruction onto stack and then jump to nn
			// nn = 2 byte imm. value. LS byte first.
			// 12 cycles
			CALL_nn = 0xCD,

			// CALL cc,nn (p.115)
			// CALL nn if condition is true (See JP cc,nn)
			// 12 cycles
			CALL_NZ_nn = 0xC4,
			CALL_Z_nn = 0xCC,
			CALL_NC_nn = 0xD4,
			CALL_C_nn = 0xDC,

			// RST n (p.116)
			// Push present address onto stack.
			// Jump to $0000 + n (subset)
			// 32 cycles
			RST_00H = 0xC7,
			RST_08H = 0xCF,
			RST_10H = 0xD7,
			RST_18H = 0xDF,
			RST_20H = 0xE7,
			RST_28H = 0xEF,
			RST_30H = 0xF7,
			RST_38H = 0xFF,

			// RET (p.117)
			// Pop two bytes from stack and jump to that address
			// 8 cycles
			RET = 0xC9,

			// RET cc (p.117)
			// Return if cc is true (Sett JP cc,nn)
			// 8 cycles
			RET_NZ = 0xC0,
			RET_Z = 0xC8,
			RET_NC = 0xD0,
			RET_C = 0xD8,

			// RETI (p.118)
			// Pop two bytes from stack, then jump to that address
			// then enable interupts
			// 8 cycles
			RETI = 0xD9,
	};

	enum class CB_Opcode_Exact : uint8_t {
		// SWAP n (p.94)
		// Swap upper and lower nibbles of n
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H, C reset
		SWAP_A = 0x37,
		SWAP_B = 0x30,
		SWAP_C = 0x31,
		SWAP_D = 0x32,
		SWAP_E = 0x33,
		SWAP_H = 0x34,
		SWAP_L = 0x35,
		SWAP_HL = 0x36, /** 16 cycles */

		// RLC n (p.101)
		// Rotate n left. Old bit 7 to Carry flag.
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H reset. C contains old bit 7
		RLC_A = 0x07,
		RLC_B = 0x00,
		RLC_C = 0x01,
		RLC_D = 0x02,
		RLC_E = 0x03,
		RLC_H = 0x04,
		RLC_L = 0x05,
		RLC_HL = 0x06, /** 16 cycles */

		// RL n (p. 102)
		// Rotate n left through Carry Flag
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H reset. C contains old bit 7
		RL_A = 0x17,
		RL_B = 0x10,
		RL_C = 0x11,
		RL_D = 0x12,
		RL_E = 0x13,
		RL_H = 0x14,
		RL_L = 0x15,
		RL_HL = 0x16, /** 16 cycles */

		// RRC n (p.103)
		// Rotate n right. Old bit 0 to Carry flag.
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H reset. C contains old bit 0
		RRC_A = 0x0F,
		RRC_B = 0x08,
		RRC_C = 0x09,
		RRC_D = 0x0A,
		RRC_E = 0x0B,
		RRC_H = 0x0C,
		RRC_L = 0x0D,
		RRC_HL = 0x0E, /** 16 cycles */

		// RR n (p. 104)
		// Rotate n right through Carry Flag
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H reset. C contains old bit 0
		RR_A = 0x1F,
		RR_B = 0x18,
		RR_C = 0x19,
		RR_D = 0x1A,
		RR_E = 0x1B,
		RR_H = 0x1C,
		RR_L = 0x1D,
		RR_HL = 0x1E, /** 16 cycles */

		// SLA n (p. 105)
		// Shift n left into Carry. LSB of n set to 0
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H reset. C contains old bit 7
		SLA_A = 0x27,
		SLA_B = 0x20,
		SLA_C = 0x21,
		SLA_D = 0x22,
		SLA_E = 0x23,
		SLA_H = 0x24,
		SLA_L = 0x25,
		SLA_HL = 0x26, /** 16 cycles */

		// SRA n (p. 106)
		// Shift n right into Carry. MSB unchanged
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H reset. C contains old bit 0
		SRA_A = 0x2F,
		SRA_B = 0x28,
		SRA_C = 0x29,
		SRA_D = 0x2A,
		SRA_E = 0x2B,
		SRA_H = 0x2C,
		SRA_L = 0x2D,
		SRA_HL = 0x2E, /** 16 cycles */

		// SRL n (p. 107)
		// Shift n right into Carry. MSB set to 0
		// 8 cycles except as marked
		// Flags: Z set if result is 0. N, H reset. C contains old bit 0
		SRL_A = 0x3F,
		SRL_B = 0x38,
		SRL_C = 0x39,
		SRL_D = 0x3A,
		SRL_E = 0x3B,
		SRL_H = 0x3C,
		SRL_L = 0x3D,
		SRL_HL = 0x3E, /** 16 cycles */

		// BIT b,r (p.108)
		// Test bit b in register r
		// b=0--7
		// 8 cycles except as marked
		// Flags: Z set if b of register r is 0. N reset. H set. C not affected
		BIT_b_A = 0x47,
		BIT_b_B = 0x40,
		BIT_b_C = 0x41,
		BIT_b_D = 0x42,
		BIT_b_E = 0x43,
		BIT_b_H = 0x44,
		BIT_b_L = 0x45,
		BIT_b_HL = 0x46, /** 16 cycles */

		// SET b,r (p.109)
		// Set bit b in register r
		// b=0--7
		// 8 cycles except as marked
		SET_b_A = 0xC7,
		SET_b_B = 0xC0,
		SET_b_C = 0xC1,
		SET_b_D = 0xC2,
		SET_b_E = 0xC3,
		SET_b_H = 0xC4,
		SET_b_L = 0xC5,
		SET_b_HL = 0xC6, /** 16 cycles */

		// RES b,r (p.110)
		// Reset bit b in register r
		// b=0--7
		// 8 cycles except as marked
		RES_b_A = 0x87,
		RES_b_B = 0x80,
		RES_b_C = 0x81,
		RES_b_D = 0x82,
		RES_b_E = 0x83,
		RES_b_H = 0x84,
		RES_b_L = 0x85,
		RES_b_HL = 0x86, /** 16 cycles */
	};
}