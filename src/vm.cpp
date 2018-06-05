#include "../include/vm.hpp"
#include "../include/op_code.hpp"

namespace gb_emu
{
	ExecuteResult VM::run()
	{

		for(;;) {
			uint8_t instruction = getByte(PC++);
			switch(static_cast<Opcode_Group>(instruction) & Opcode_Group::MASK) {
			case Opcode_Group::MISC1:
				break;
			case Opcode_Group::LD:
			{
				if(static_cast<Opcode>(instruction) == Opcode::HALT) {
					// Halt
				}
				else {
					Opcode_Register r1 = static_cast<Opcode_Register>((instruction >> 3) | 0x07);
					Opcode_Register r2 = static_cast<Opcode_Register>(instruction | 0x07);
					uint8_t value = readValue(r2);
					writeValue(r1, value);
				}
			}
				break;
			case Opcode_Group::ARITH:
			{
				Opcode_Arithmetic_Command cmd = static_cast<Opcode_Arithmetic_Command>((instruction >> 3) | 0x07);
				Opcode_Register reg = static_cast<Opcode_Register>(instruction | 0x07);

				uint8_t value = readValue(reg);

				uint8_t regA = getRegister(Register::A);
				switch(cmd)
				{
				case Opcode_Arithmetic_Command::ADD:
					// Perform an ADC with no carry bit
					clearFlag(Flag::C);
					ADC(value);
					break;
				case Opcode_Arithmetic_Command::ADC:
					ADC(value);
					break;
				case Opcode_Arithmetic_Command::SUB:
					// Perform an SBC with "no" carry bit (Since it is
					// sub, we invert the meaning of carry and so set C)
					setFlag(Flag::C);
					ADC(~value);
					toggleFlag(Flag::C);
					setFlag(Flag::N);
					break;
				case Opcode_Arithmetic_Command::SBC:
					// a - b - c = a + ~b + 1 - c = a + ~b + !c
					toggleFlag(Flag::C);
					ADC(~value);
					toggleFlag(Flag::C);
					setFlag(Flag::N);
					break;
				}
			}
				break;
			case Opcode_Group::MISC2:
				break;
			default:
				return ExecuteResult::RUNTIME_ERROR;
			}
		}
		return ExecuteResult();
	}

	uint8_t VM::readValue(Opcode_Register r) const
	{
		if(r == Opcode_Register::HL) {
			return getByte(registerPairs[toUType(RegisterPair::HL)]);
		}
		else {
			return getRegister(getRegister_from_OpcodeRegister(r));
		}
	}
	void VM::writeValue(Opcode_Register r, uint8_t value)
	{
		if(r == Opcode_Register::HL) {
			setByte(registerPairs[toUType(RegisterPair::HL)], value);
		}
		else {
			setRegister(getRegister_from_OpcodeRegister(r), value);
		}
	}
	void VM::ADC(uint8_t b)
	{
		uint8_t regA = getRegister(Register::A);
		uint8_t outValue;
		if(getFlag(Flag::C))
		{
			setFlag(Flag::C, (regA >= 0xFF - b));
			outValue = regA + b + 1;
		}
		else
		{
			setFlag(Flag::C, (regA > 0xFF - b));
			outValue = regA + b;
		}
		uint8_t carryIns = regA ^ b ^ outValue;
		setFlag(Flag::H, ((carryIns >> 4) & 1));
		setRegister(Register::A, outValue);
		clearFlag(Flag::N);
	}
}
