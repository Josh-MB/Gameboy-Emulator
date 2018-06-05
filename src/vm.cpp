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
			{
				Opcode_Misc1_Command_Groups cmd = static_cast<Opcode_Misc1_Command_Groups>(instruction | 0x0F);
				switch(cmd)
				{
				case Opcode_Misc1_Command_Groups::MISC1:
					break;
				case Opcode_Misc1_Command_Groups::LD_r1_d16:
					break;
				case Opcode_Misc1_Command_Groups::LD_add_A:
					break;
				case Opcode_Misc1_Command_Groups::INC_rr:
				{
					Opcode_Register_Pair rr = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					uint16_t value = readValue(rr);
					writeValue(rr, ++value);
				}
					break;
				case Opcode_Misc1_Command_Groups::INC_r_1:
				case Opcode_Misc1_Command_Groups::INC_r_2:
				{
					Opcode_Register r = static_cast<Opcode_Register>((instruction >> 3) | 0x07);
					clearFlag(Flag::C);
					ADC(getRegister_from_OpcodeRegister(r), 1);
				}
					break;
				case Opcode_Misc1_Command_Groups::DEC_r_1:
				case Opcode_Misc1_Command_Groups::DEC_r_2:
				{
					Opcode_Register r = static_cast<Opcode_Register>((instruction >> 3) | 0x07);
					clearFlag(Flag::C);
					SBC(getRegister_from_OpcodeRegister(r), 1);
				}
					break;
				case Opcode_Misc1_Command_Groups::LD_r1_d8_1:
				case Opcode_Misc1_Command_Groups::LD_r1_d8_2:
				{
					Opcode_Register r = static_cast<Opcode_Register>((instruction >> 3) | 0x07);
					uint8_t value = getByte(PC++);
					writeValue(r, value);
				}
					break;
				case Opcode_Misc1_Command_Groups::MISC2:
					break;
				case Opcode_Misc1_Command_Groups::MISC3:
					break;
				case Opcode_Misc1_Command_Groups::ADD_HL_rr1:
				{
					Opcode_Register_Pair rr = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					uint16_t value = readValue(rr);
					uint16_t HLvalue = getRegister(RegisterPair::HL);
					setFlag(Flag::C, (HLvalue > 0xFFFF - value));
					uint16_t outValue = HLvalue + value;
					uint16_t carryIns = HLvalue ^ value ^ outValue;
					setFlag(Flag::H, ((carryIns >> 12) & 1));
					setRegister(RegisterPair::HL, outValue);
					clearFlag(Flag::N);
					setFlag(Flag::Z, outValue == 0);
				}
					break;
				case Opcode_Misc1_Command_Groups::LD_A_add:
					break;
				case Opcode_Misc1_Command_Groups::DEC_rr:
					Opcode_Register_Pair rr = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					uint16_t value = readValue(rr);
					writeValue(rr, --value);
					break;
				case Opcode_Misc1_Command_Groups::MISC4:
					break;
				}
			}
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
					ADC(Register::A, value);
					break;
				case Opcode_Arithmetic_Command::ADC:
					ADC(Register::A, value);
					break;
				case Opcode_Arithmetic_Command::SUB:
					// Perform an SBC with "no" carry bit (Since it is
					// sub, we invert the meaning of carry and so set C)
					clearFlag(Flag::C);
					SBC(Register::A, value);
					break;
				case Opcode_Arithmetic_Command::SBC:
					SBC(Register::A, value);
					break;
				case Opcode_Arithmetic_Command::AND:
				{
					clearFlags();
					uint8_t out = value & regA;
					setRegister(Register::A, out);
					setFlag(Flag::H);
					setFlag(Flag::Z, out == 0);
				}
					break;
				case Opcode_Arithmetic_Command::XOR:
				{
					clearFlags();
					uint8_t out = value ^ regA;
					setRegister(Register::A, out);
					setFlag(Flag::Z, out == 0);
				}
				break;
				case Opcode_Arithmetic_Command::OR:
				{
					clearFlags();
					uint8_t out = value | regA;
					setRegister(Register::A, out);
					setFlag(Flag::Z, out == 0);
				}
				break;
				case Opcode_Arithmetic_Command::CP:
					// CP is just a sub command, but with the result thrown away
					// so we reset A to its original value
					clearFlags();
					SBC(Register::A, value);
					setRegister(Register::A, regA);
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
	uint16_t VM::readValue(Opcode_Register_Pair r) const
	{
		if(r == Opcode_Register_Pair::SP) {
			return SP;
		}
		return getRegister(getRegisterPair_from_OpcodeRegisterPair(r));
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

	void VM::writeValue(Opcode_Register_Pair r, uint16_t value)
	{
		if(r == Opcode_Register_Pair::SP) {
			SP = value;
		}
		else {
			setRegister(getRegisterPair_from_OpcodeRegisterPair(r), value);
		}
	}


	void VM::ADC(Register r, uint8_t b)
	{
		uint8_t regValue = getRegister(r);
		uint8_t outValue;
		if(getFlag(Flag::C))
		{
			setFlag(Flag::C, (regValue >= 0xFF - b));
			outValue = regValue + b + 1;
		}
		else
		{
			setFlag(Flag::C, (regValue > 0xFF - b));
			outValue = regValue + b;
		}
		uint8_t carryIns = regValue ^ b ^ outValue;
		setFlag(Flag::H, ((carryIns >> 4) & 1));
		setRegister(r, outValue);
		clearFlag(Flag::N);
		setFlag(Flag::Z, outValue == 0);
	}
	void VM::SBC(Register r, uint8_t b)
	{
		// a - b - c = a + ~b + 1 - c = a + ~b + !c
		toggleFlag(Flag::C);
		ADC(r, ~b);
		toggleFlag(Flag::C);
		setFlag(Flag::N);
	}
}
