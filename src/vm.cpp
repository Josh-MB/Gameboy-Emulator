#include "../include/vm.hpp"
#include "../include/op_code.hpp"

namespace gb_emu
{
	ExecuteResult VM::run()
	{

		for(;;) {
			uint8_t instruction = getByte(PC++);
			switch(static_cast<Opcode_Group>(instruction) & Opcode_Group::MASK)
			{
			case Opcode_Group::MISC1:
			{
				Opcode_Misc1_Command_Groups cmd = static_cast<Opcode_Misc1_Command_Groups>(instruction | 0x0F);
				switch(cmd)
				{
				case Opcode_Misc1_Command_Groups::MISC1:
				{
					Opcode_Exact op = static_cast<Opcode_Exact>(instruction);
					switch(op)
					{
					case Opcode_Exact::NOP:
						// Do Noop for 4 cycles
						break;
					case Opcode_Exact::STOP:
						// Halt CPU and LCD until button press (interrupt?)
						break;
					case Opcode_Exact::JR_NZ_n:
					{
						uint8_t offset = getByte(PC++);
						if(!getFlag(Flag::Z)) {
							shortJump(offset);
						}
					}
					break;
					case Opcode_Exact::JR_NC_n:
					{
						uint8_t offset = getByte(PC++);
						if(!getFlag(Flag::C)) {
							shortJump(offset);
						}
					}
					break;
					}
				}
					break;
				case Opcode_Misc1_Command_Groups::LD_r1_d16:
				{
					Opcode_Register_Pair reg = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					uint8_t valueHigh = getByte(PC++);
					uint8_t valueLow = getByte(PC++);
					writeValue(reg, (valueHigh << 8) | valueLow);
				}
					break;
				case Opcode_Misc1_Command_Groups::LD_add_A:
				{
					Opcode_Register_Pair_Address reg = static_cast<Opcode_Register_Pair_Address>((instruction >> 4) | 0x03);
					uint8_t value = getRegister(Register::A);
					uint16_t addr = readValue(reg);
					if(reg == Opcode_Register_Pair_Address::HL_plus)
						setRegister(RegisterPair::HL, getRegister(RegisterPair::HL) + 1);
					else if(reg == Opcode_Register_Pair_Address::HL_minus)
						setRegister(RegisterPair::HL, getRegister(RegisterPair::HL) - 1);
					setByte(addr, value);
				}
					break;
				case Opcode_Misc1_Command_Groups::INC_rr:
				{
					Opcode_Register_Pair reg = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					uint16_t value = readValue(reg);
					writeValue(reg, ++value);
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
				{
					Opcode_Exact op = static_cast<Opcode_Exact>(instruction);
					switch(op)
					{
					case Opcode_Exact::RLCA:
					{
						clearFlags();
						uint8_t regA = getRegister(Register::A);
						setFlag(Flag::C, regA & 0x80);
						regA <<= 1;
						setFlag(Flag::Z, regA == 0);
						setRegister(Register::A, regA);
					}
					break;
					case Opcode_Exact::RLA:
					{
						bool hadCarry = getFlag(Flag::C);
						clearFlags();
						uint8_t regA = getRegister(Register::A);
						setFlag(Flag::C, regA & 0x80);
						regA <<= 1;
						if(hadCarry) {
							regA |= 0x1;
						}
						setFlag(Flag::Z, regA == 0);
						setRegister(Register::A, regA);
					}
					break;
					case Opcode_Exact::DAA:
						DAA();
					break;
					case Opcode_Exact::SCF:
						setFlag(Flag::C);
					break;
					}
				}
					break;
				case Opcode_Misc1_Command_Groups::MISC3:
				{
					Opcode_Exact op = static_cast<Opcode_Exact>(instruction);
					switch(op)
					{
					case Opcode_Exact::LD_nn_SP:
					{
						uint8_t addrHigh = getByte(PC++);
						uint8_t addrLow = getByte(PC++);
						uint16_t addr = (addrHigh << 8) | addrLow;
						setByte(addr, SP >> 8);
						setByte(++addr, static_cast<uint8_t>(SP & 0xFF));
					}
						break;
					case Opcode_Exact::JR_n:
					{
						uint8_t offset = getByte(PC++);
						shortJump(offset);
					}
						break;
					case Opcode_Exact::JR_Z_n:
					{
						uint8_t offset = getByte(PC++);
						if(getFlag(Flag::Z)) {
							shortJump(offset);
						}
					}
					break;
					case Opcode_Exact::JR_C_n:
					{
						uint8_t offset = getByte(PC++);
						if(getFlag(Flag::C)) {
							shortJump(offset);
						}
					}
					break;
					}
				}
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
				{
					Opcode_Register_Pair_Address reg = static_cast<Opcode_Register_Pair_Address>((instruction >> 4) | 0x03);
					uint16_t addr = readValue(reg);
					uint8_t value = getByte(addr);
					if(reg == Opcode_Register_Pair_Address::HL_plus)
						setRegister(RegisterPair::HL, getRegister(RegisterPair::HL) + 1);
					else if(reg == Opcode_Register_Pair_Address::HL_minus)
						setRegister(RegisterPair::HL, getRegister(RegisterPair::HL) - 1);
					setRegister(Register::A, value);
				}
				break;
				case Opcode_Misc1_Command_Groups::DEC_rr:
				{
					Opcode_Register_Pair rr = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					uint16_t value = readValue(rr);
					writeValue(rr, --value);
				}
					break;
				case Opcode_Misc1_Command_Groups::MISC4:
				{
					Opcode_Exact op = static_cast<Opcode_Exact>(instruction);
					switch(op)
					{
					case Opcode_Exact::RRCA:
					{
						clearFlags();
						uint8_t regA = getRegister(Register::A);
						setFlag(Flag::C, regA & 0x1);
						regA >>= 1;
						setFlag(Flag::Z, regA == 0);
						setRegister(Register::A, regA);
					}
					break;
					case Opcode_Exact::RRA:
					{
						bool hadCarry = getFlag(Flag::C);
						clearFlags();
						uint8_t regA = getRegister(Register::A);
						setFlag(Flag::C, regA & 0x1);
						regA >>= 1;
						if(hadCarry) {
							regA |= 0x80;
						}
						setFlag(Flag::Z, regA == 0);
						setRegister(Register::A, regA);
					}
					break;
					case Opcode_Exact::CPL:
						setRegister(Register::A, ~getRegister(Register::A));
						break;
					case Opcode_Exact::CCF:
						toggleFlag(Flag::C);
						break;
					}
				}
					break;
				}
			}
				break;
			case Opcode_Group::LD:
			{
				if(static_cast<Opcode>(instruction) == Opcode::HALT) {
					// Halt. Power down CPU until interrupt occurs
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
			{
				Opcode_Misc2_Command_Groups cmd = static_cast<Opcode_Misc2_Command_Groups>(instruction | 0x0F);
				switch(cmd)
				{
				case Opcode_Misc2_Command_Groups::POP:
				{
					Opcode_Register_Pair rr = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					writeValue(rr, pop_double());
				}
					break;
				case Opcode_Misc2_Command_Groups::PUSH:
				{
					Opcode_Register_Pair rr = static_cast<Opcode_Register_Pair>((instruction >> 4) | 0x03);
					push_double(readValue(rr));
				}
					break;
				case Opcode_Misc2_Command_Groups::ARITH_1:
				case Opcode_Misc2_Command_Groups::ARITH_2:
				{
					uint8_t value = getByte(PC++);
					uint8_t regA = getRegister(Register::A);
					Opcode_Arithmetic_Command arith_cmd = static_cast<Opcode_Arithmetic_Command>((instruction >> 3) | 0x07);
					switch(arith_cmd)
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
				case Opcode_Misc2_Command_Groups::RST_1:
				case Opcode_Misc2_Command_Groups::RST_2:
				{
					uint8_t rst_address = (instruction >> 3) & 0xF;
					push_double(PC);
					longJump(rst_address);
				}
					break;
				default:
				{
					Opcode_Exact op = static_cast<Opcode_Exact>(instruction);
					switch(op)
					{
					case Opcode_Exact::RET_NZ:
						if(!getFlag(Flag::Z)) ret();
						break;
					case Opcode_Exact::RET_NC:
						if(!getFlag(Flag::C)) ret();
						break;
					case Opcode_Exact::RET_Z:
						if(getFlag(Flag::Z)) ret();
						break;
					case Opcode_Exact::RET_C:
						if(getFlag(Flag::C)) ret();
						break;
					case Opcode_Exact::RET:
						ret();
						break;
					case Opcode_Exact::RETI:
						ret();
						// todo: enable interrupts
						break;
					case Opcode_Exact::JP_NZ_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(!getFlag(Flag::Z)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_NC_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(!getFlag(Flag::C)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_Z_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(getFlag(Flag::Z)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_C_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(getFlag(Flag::C)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						longJump(addr);
					}
					break;
					case Opcode_Exact::JP_HL:
						longJump(getRegister(RegisterPair::HL));
						break;
					case Opcode_Exact::CALL_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						call(addr);
					}
						break;
					case Opcode_Exact::CALL_NZ_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(!getFlag(Flag::Z)) call(addr);
					}
					break;
					case Opcode_Exact::CALL_NC_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(!getFlag(Flag::C)) call(addr);
					}
					break;
					case Opcode_Exact::CALL_Z_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(getFlag(Flag::Z)) call(addr);
					}
					break;
					case Opcode_Exact::CALL_C_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						if(getFlag(Flag::C)) call(addr);
					}
					break;
					case Opcode_Exact::LDH_n_A:
						setByte(getByte(PC++), getRegister(Register::A));
					break;
					case Opcode_Exact::LDH_A_n:
						setRegister(Register::A, getByte(getByte(PC++)));
						break;
					case Opcode_Exact::LD_offsetC_A:
						setByte(getRegister(Register::C), getRegister(Register::A));
						break;
					case Opcode_Exact::LD_A_offsetC:
						setRegister(Register::A, getByte(getRegister(Register::C)));
						break;
					case Opcode_Exact::LD_nn_A:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						setRegister(Register::A, getByte(addr));
					}
					break;
					case Opcode_Exact::LD_A_nn:
					{
						uint16_t addr = getByte(PC++);
						addr |= (getByte(PC++) << 8);
						setByte(addr, getRegister(Register::A));
					}
					break;
					case Opcode_Exact::LD_SP_HL:
						SP = getRegister(RegisterPair::HL);
						break;
					case Opcode_Exact::LDHL_SP_n:
					{
						clearFlags();
						uint8_t offset = getByte(PC++);
						uint16_t addr = SP + offset;
						setRegister(RegisterPair::HL, addr);
						uint16_t carry = addr ^ SP ^ offset;
						setFlag(Flag::H, carry & 0x10);
						setFlag(Flag::C, SP > 0xFFFF - offset);
					}
					break;
					}
				}
					break;
				}
			}
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
	uint16_t VM::readValue(Opcode_Register_Pair_Address r) const
	{
		if(r == Opcode_Register_Pair_Address::HL_plus || r == Opcode_Register_Pair_Address::HL_minus) {
			return getRegister(RegisterPair::HL);
		}
		return getRegister(getRegisterPair_from_OpcodeRegisterPair(static_cast<Opcode_Register_Pair>(r)));
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

	/** 
	 * Decimal adjust register A. Handles converting register A
	 * to Binary Coded Decimal format. See http://www.z80.info/z80syntx.htm#DAA
	 * and https://www.reddit.com/r/EmuDev/comments/4ycoix/a_guide_to_the_gameboys_halfcarry_flag/
	 */
	void VM::DAA()
	{
		uint8_t offset = 0;
		uint8_t regA = getRegister(Register::A);
		bool N = getFlag(Flag::N);
		if(getFlag(Flag::H) || (!N && (regA & 0xF) > 9)) {
			offset = 6;
		}
		if(getFlag(Flag::C) || (!N && regA > 0x99)) {
			offset |= 0x60;
			setFlag(Flag::C);
		}
		setRegister(Register::A, regA + (N ? -offset : offset));
		setFlag(Flag::Z, getRegister(Register::A) == 0);
		clearFlag(Flag::H);
	}

	void VM::push(uint8_t value)
	{
		setByte(--SP, value);
	}
	void VM::push_double(uint16_t value)
	{
		push(value & 0xFF); // Low byte
		push(value >> 8); // High byte
	}
	uint8_t VM::pop()
	{
		return getByte(SP++);
	}
	uint16_t VM::pop_double()
	{
		uint16_t ret = (pop() << 8);
		ret |= pop();
		return ret;
	}
	void VM::ret()
	{
		PC = pop_double();
	}
	void VM::call(uint16_t addr)
	{
		push_double(PC + 1);
		longJump(addr);
	}
}
