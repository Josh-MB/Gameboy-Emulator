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
				{
					setRegister(Register::A, regA + value);
					(regA + value == 0) ? setFlag(Flag::Z) : clearFlag(Flag::Z);
					uint16_t bigRegA = regA, bigValue = value;
					regA &= 0x0F;
					value &= 0x0F;
					(regA + value > 0x0F) ? setFlag(Flag::H) : clearFlag(Flag::H);
					(bigRegA + bigValue > 0xF0) ? setFlag(Flag::C) : clearFlag(Flag::C);
					clearFlag(Flag::N);
				}
					break;
				case Opcode_Arithmetic_Command::ADC:
				{
					uint8_t carry = getFlag(Flag::C);
					setRegister(Register::A, regA + value + carry);
					(regA + value + carry == 0) ? setFlag(Flag::Z) : clearFlag(Flag::Z);
					uint16_t bigRegA = regA, bigValue = value;
					regA &= 0x0F;
					value &= 0x0F;
					(regA + value + carry > 0x0F) ? setFlag(Flag::H) : clearFlag(Flag::H);
					(bigRegA + bigValue + carry > 0xF0) ? setFlag(Flag::C) : clearFlag(Flag::C);
					clearFlag(Flag::N);
				}
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
}
