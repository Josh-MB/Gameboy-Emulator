#pragma once

#include "common.hpp"
#include "op_code.hpp"
#include "debug.hpp"
#include "mem.hpp"
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

	inline Opcode_Register decodeRegister(uint8_t instruction, bool lower = false) {
		if(!lower) instruction >>= 3;
		return static_cast<Opcode_Register>(instruction & 0x07);
	}

	inline Opcode_Register_Pair decodeRegisterPair(uint8_t instruction) {
		return static_cast<Opcode_Register_Pair>((instruction >> 4) & 0x3);
	}

	inline Opcode_Register_Pair_Address decodeRegisterPairAddress(uint8_t instruction) {
		return static_cast<Opcode_Register_Pair_Address>((instruction >> 4) & 0x03);
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
		VM() { mem.loadFromFile("DMG_ROM.bin"); }
		ExecuteResult run();
	private:

		uint16_t SP = 0xFFFE;
		uint16_t PC = 0;

		// Union for handling 8 bit registers and addressing them as pairs
		union {
			// Last two registers are fake registers such that
			// getRegister[HL_UNUSED_UPPER/LOWER] does not corrupt memory
			uint8_t registers[10];
			uint16_t registerPairs[5];
		};

		MMU mem;
		
		/**
		 * Fetches the next byte and increments the program counter
		 */
		uint8_t fetchByte() {
			return mem.getByte(PC++);
		}

		/**
		 * Fetches the next double byte, and increments program counter
		 * twice. Assumes LSB stored first.
		 */
		uint16_t fetchDouble() {
			uint16_t ret = mem.getDouble(PC);
			PC += 2;
			return ret;
		}
		/**
		 * Gets the byte referenced by the opcode register. This could be (HL) which
		 * is actually a memory access (where HL stores the pointer)
		 */
		uint8_t readValue(Opcode_Register r) const;
		/**
		* Gets the byte referenced by the opcode register pair. This could be SP which
		* is actually the stack pointer rather than a register pair
		*/
		uint16_t readValue(Opcode_Register_Pair r) const;
		/**
		* Gets the byte referenced by the opcode register pair. This could be HL+ or HL- which
		* which both refer to HL, which is modified after the command (done later as this is a
		* const function
		*/
		uint16_t readValue(Opcode_Register_Pair_Address r) const;
		/**
		 * Writes the byte to the target specified. Usually a register, unless
		 * r = (HL), in which case it's a memory address (where HL stores the pointer)
		 */
		void writeValue(Opcode_Register r, uint8_t value);
		/**
		* Writes the byte to the target specified. Usually a register, but could
		* be the stack pointer
		*/
		void writeValue(Opcode_Register_Pair r, uint16_t value);

		/**
		 * Get the value stored in the register
		 */
		uint8_t getRegister(Register r) const {
			CHECK_REGISTER(r);
			return registers[toUType(r)];
		}

		/**
		 * Get the value stored in the register pair
		 */
		uint16_t getRegister(RegisterPair r) const {
			return registerPairs[toUType(r)];
		}

		/**
		 * Put byte into register
		 */
		void setRegister(Register r, uint8_t value) {
			CHECK_REGISTER(r);
			registers[toUType(r)] = value;
		}

		/**
		 * Put double byte into register pair
		 */
		void setRegister(RegisterPair r, uint16_t value) {
			registerPairs[toUType(r)] = value;
		}

		/**
		 * Set flag if state otherwise clear
		 */
		void setFlag(Flag f, bool state) {
			registers[toUType(Register::F)] |= (state * toUType(f));
		}

		/**
		 * Set flag
		 */
		void setFlag(Flag f) {
			registers[toUType(Register::F)] |= toUType(f);
		}

		/**
		 * Clear flag
		 */
		void clearFlag(Flag f) {
			registers[toUType(Register::F)] &= ~(toUType(f));
		}

		/**
		 * Clear all flags
		 */
		void clearFlags() {
			registers[toUType(Register::F)] = 0;
		}

		/**
		 * Toggle flag
		 */
		void toggleFlag(Flag f) {
			registers[toUType(Register::F)] ^= toUType(f);
		}

		/**
		 * Get flag state
		 */
		bool getFlag(Flag f) const {
			return (registers[toUType(Register::F)] & toUType(f)) != 0;
		}

		/**
		 * Increment program counter by offset
		 */
		void shortJump(uint8_t offset) {
			PC += offset;
		}

		/**
		 * Set program counter to addr
		 */
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
		void call(uint16_t addr);

		void enableInterrupts();
		void disableInterrupts();

		void doPrefixCBCommand();
		void doArithmeticCommand(Opcode_Arithmetic_Command cmd, uint8_t operand);

		/**
		 * Adds two values and returns the result
		 * while also calculating the carry and half
		 * carry flags
		 */
		uint8_t addAndCalcCarry(uint8_t a, uint8_t b);
		uint16_t addAndCalcCarry(uint16_t a, uint16_t b);

		/**
		 * Rotates the register left or right, and through the carry bit or not
		 */
		void rotate(Opcode_Register r, bool right, bool throughCarry);
	};
}