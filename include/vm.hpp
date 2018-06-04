#pragma once

#include <cstdint>
#include "common.hpp"

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
	};

	enum class RegisterPair : uint8_t {
		BC = 0,
		DE = 1,
		HL = 2,
		AF = 3,
	};

	enum class Flag : uint8_t {
		Z = 7, // Set if result is zero
		N = 6, // Set if subtract
		H = 5, // Set if half carry
		C = 4, // Set if carry
	};

	enum class ExecuteResult {
		OK = 0,
		RUNTIME_ERROR,
	};

	class VM {
	public:
	private:
		ExecuteResult run();

		uint16_t SP;
		uint16_t PC;

		union {
			uint8_t registers[8];
			uint16_t registerPairs[4];
		};

		// 16 bytes of memory
		uint8_t memory[0xFFFF];

		uint8_t getRegister(Register r) const {
			return registers[toUType(r)];
		}

		uint16_t getRegister(RegisterPair r) const {
			return registerPairs[toUType(r)];
		}

		void setRegister(Register r, uint8_t value) {
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
	};
}