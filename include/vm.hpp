#pragma once

#include <cstdint>

namespace gb_emu
{
	enum class Register : uint8_t {
		A = 0,
		F,
		B,
		C,
		D,
		E,
		H,
		L,
		count,
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

		uint16_t *SP;
		uint16_t *PC;

		uint8_t registers[static_cast<int>(Register::count)];

		// 16 bytes of memory
		uint8_t memory[0xFFFF];
	};
}