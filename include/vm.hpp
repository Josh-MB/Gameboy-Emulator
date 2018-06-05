#pragma once

#include "common.hpp"
#include "op_code.hpp"
#include "debug.hpp"
#include <cstdint>

namespace gb_emu
{
	enum class Register : uint8_t {
		B = 0,
		C,
		D,
		E,
		H,
		L,
		A,
		F,
		HL_UNUSED_UPPER, // Fake registers to make converting from opcode register
		HL_UNUSED_LOWER, // to actual registers easier
	};

	enum class RegisterPair : uint8_t {
		BC = 0,
		DE = 1,
		HL = 2,
		AF = 3,
		UNUSED = 4,
	};

	constexpr Register getRegister_from_OpcodeRegister(Opcode_Register r) {
		if(r == Opcode_Register::A) {
			return Register::A;
		}
		else if(r == Opcode_Register::HL) {
			return Register::HL_UNUSED_UPPER;
		}
		else
			return static_cast<Register>(r);
	}

	constexpr RegisterPair getRegisterPair_from_OpcodeRegisterPair(Opcode_Register_Pair r) {
		if(r == Opcode_Register_Pair::SP) {
			return RegisterPair::UNUSED;
		}
		else
			return static_cast<RegisterPair>(r);
	}

	enum class Flag : uint8_t {
		Z = 1<<7, // Set if result is zero
		N = 1<<6, // Set if subtract
		H = 1<<5, // Set if half carry
		C = 1<<4, // Set if carry
	};

	enum class ExecuteResult {
		OK = 0,
		RUNTIME_ERROR,
	};

	class VM {
	public:
	private:
		ExecuteResult run();

		uint16_t SP = 0xFFFE;
		uint16_t PC = 0;

		// Union for handling 8 bit registers and addressing them as pairs
		union {
			// Last two registers are fake registers such that
			// getRegister[HL_UNUSED_UPPER/LOWER] does not corrupt memory
			uint8_t registers[10];
			uint16_t registerPairs[5];
		};


		
		// 16 bytes of memory
		uint8_t memory[0xFFFF];

		/**
		 * Gets the byte referenced by the opcode register. This could be (HL) which
		 * is actually a memory access (where HL stores the pointer)
		 */
		uint8_t readValue(Opcode_Register r) const;
		uint16_t readValue(Opcode_Register_Pair r) const;
		uint16_t readValue(Opcode_Register_Pair_Address r) const;
		/**
		 * Writes the byte to the target specified. Usually a register, unless
		 * r = (HL), in which case it's a memory address (where HL stores the pointer)
		 */
		void writeValue(Opcode_Register r, uint8_t value);
		void writeValue(Opcode_Register_Pair r, uint16_t value);

		uint8_t getRegister(Register r) const {
			CHECK_REGISTER(r);
			return registers[toUType(r)];
		}

		uint16_t getRegister(RegisterPair r) const {
			return registerPairs[toUType(r)];
		}

		void setRegister(Register r, uint8_t value) {
			CHECK_REGISTER(r);
			registers[toUType(r)] = value;
		}

		void setRegister(RegisterPair r, uint16_t value) {
			registerPairs[toUType(r)] = value;
		}

		uint8_t getByte(uint8_t address) const {
			return memory[0xFF00 + address];
		}

		uint8_t getByte(uint16_t address) const {
			return memory[address];
		}

		void setByte(uint8_t address, uint8_t value) {
			memory[0xFF00 + address] = value;
		}

		void setByte(uint16_t address, uint8_t value) {
			memory[address] = value;
		}

		void setFlag(Flag f, bool state) {
			registers[toUType(Register::F)] |= (state * toUType(f));
		}

		void setFlag(Flag f) {
			registers[toUType(Register::F)] |= toUType(f);
		}

		void clearFlag(Flag f) {
			registers[toUType(Register::F)] &= ~(toUType(f));
		}

		void clearFlags() {
			registers[toUType(Register::F)] = 0;
		}

		void toggleFlag(Flag f) {
			registers[toUType(Register::F)] ^= toUType(f);
		}

		bool getFlag(Flag f) {
			return (registers[toUType(Register::F)] & toUType(f)) != 0;
		}

		void shortJump(uint8_t offset) {
			PC += offset;
		}

		void longJump(uint16_t addr) {
			PC = addr;
		}

		void ADC(Register r, uint8_t b);
		void SBC(Register r, uint8_t b);
		void DAA();

		void push(uint8_t value);
		void push_double(uint16_t value);

		uint8_t pop();
		uint16_t pop_double();

		void ret();
	};
}