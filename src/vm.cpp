#include "../include/vm.hpp"
#include "../include/op_code.hpp"

namespace gb_emu
{
	ExecuteResult VM::run()
	{

		for(;;) {
			uint8_t instruction = fetchByte();
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
						uint8_t offset = fetchByte();
						if(!getFlag(Flag::Z)) {
							shortJump(offset);
						}
					}
					break;
					case Opcode_Exact::JR_NC_n:
					{
						uint8_t offset = fetchByte();
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
					writeValue(reg, fetchDouble());
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
					uint8_t value = fetchByte();
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
						if(getFlag(Flag::C)) {
							regA |= 0x1;
						}
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
						setDouble(fetchDouble(), SP);
						break;
					case Opcode_Exact::JR_n:
						shortJump(fetchByte());
						break;
					case Opcode_Exact::JR_Z_n:
					{
						uint8_t offset = fetchByte();
						if(getFlag(Flag::Z)) {
							shortJump(offset);
						}
					}
					break;
					case Opcode_Exact::JR_C_n:
					{
						uint8_t offset = fetchByte();
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
					uint16_t outValue = addAndCalcCarry(HLvalue, value);
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
						if(getFlag(Flag::C)) {
							regA |= 0x80;
						}
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
					uint8_t value = fetchByte();
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
						uint16_t addr = fetchDouble();
						if(!getFlag(Flag::Z)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_NC_nn:
					{
						uint16_t addr = fetchDouble();
						if(!getFlag(Flag::C)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_Z_nn:
					{
						uint16_t addr = fetchDouble();
						if(getFlag(Flag::Z)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_C_nn:
					{
						uint16_t addr = fetchDouble();
						if(getFlag(Flag::C)) longJump(addr);
					}
					break;
					case Opcode_Exact::JP_nn:
						longJump(fetchDouble());
						break;
					case Opcode_Exact::JP_HL:
						longJump(getRegister(RegisterPair::HL));
						break;
					case Opcode_Exact::CALL_nn:
						call(fetchDouble());
						break;
					case Opcode_Exact::CALL_NZ_nn:
					{
						uint16_t addr = fetchDouble();
						if(!getFlag(Flag::Z)) call(addr);
					}
					break;
					case Opcode_Exact::CALL_NC_nn:
					{
						uint16_t addr = fetchDouble();
						if(!getFlag(Flag::C)) call(addr);
					}
					break;
					case Opcode_Exact::CALL_Z_nn:
					{
						uint16_t addr = fetchDouble();
						if(getFlag(Flag::Z)) call(addr);
					}
					break;
					case Opcode_Exact::CALL_C_nn:
					{
						uint16_t addr = fetchDouble();
						if(getFlag(Flag::C)) call(addr);
					}
					break;
					case Opcode_Exact::LDH_n_A:
						setByte(fetchByte(), getRegister(Register::A));
					break;
					case Opcode_Exact::LDH_A_n:
						setRegister(Register::A, getByte(fetchByte()));
						break;
					case Opcode_Exact::LD_offsetC_A:
						setByte(getRegister(Register::C), getRegister(Register::A));
						break;
					case Opcode_Exact::LD_A_offsetC:
						setRegister(Register::A, getByte(getRegister(Register::C)));
						break;
					case Opcode_Exact::LD_nn_A:
						setByte(fetchDouble(), getRegister(Register::A));
						break;
					case Opcode_Exact::LD_A_nn:
						setRegister(Register::A, getByte(fetchDouble()));
						break;
					case Opcode_Exact::LD_SP_HL:
						SP = getRegister(RegisterPair::HL);
						break;
					case Opcode_Exact::LDHL_SP_n:
					{
						// This assumes the command just stores the address SP+n into HL, not *(SP+n)
						clearFlags();
						uint8_t offset = fetchByte();
						uint16_t value = addAndCalcCarry(SP, static_cast<uint16_t>(offset));
						setRegister(RegisterPair::HL, value);
					}
					break;
					case Opcode_Exact::ADD_SP_n:
					{
						clearFlags();
						uint8_t offset = fetchByte();
						SP = addAndCalcCarry(SP, static_cast<uint16_t>(offset));
					}
					break;
					case Opcode_Exact::DI:
						disableInterrupts();
						break;
					case Opcode_Exact::EI:
						enableInterrupts();
						break;
					case Opcode_Exact::CB:
						doPrefixCBCommand();
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
		setFlag(Flag::H, carryIns & 0x10);
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
	void VM::enableInterrupts()
	{
	}
	void VM::disableInterrupts()
	{
	}
	void VM::doPrefixCBCommand()
	{
		uint8_t instruction = fetchByte();
		Opcode_Register reg = static_cast<Opcode_Register>(instruction & 0x3);
		switch(static_cast<Opcode_Prefix_Group>(instruction) & Opcode_Prefix_Group::MASK)
		{
		case Opcode_Prefix_Group::MISC1:
		{
			bool bit3 = instruction & 0x08;
			switch(static_cast<Opcode_Prefix_Misc1_Command_Groups>(instruction) & Opcode_Prefix_Misc1_Command_Groups::MASK)
			{
			case Opcode_Prefix_Misc1_Command_Groups::ROTATE:
			{
				clearFlags();
				uint8_t carryMask = bit3 ? 0x1 : 0x80;
				uint8_t rotateInMask = bit3 ? 0x80 : 0x1;
				uint8_t regVal = readValue(reg);
				setFlag(Flag::C, regVal & carryMask);
				if(bit3)
					regVal >>= 1;
				else
					regVal <<= 1;
				if(getFlag(Flag::C)) {
					regVal |= rotateInMask;
				}
				setFlag(Flag::Z, regVal == 0);
				writeValue(reg, regVal);
			}
				break;
			case Opcode_Prefix_Misc1_Command_Groups::ROTATE_THRU_CARRY:
			{
				bool hadCarry = getFlag(Flag::C);
				clearFlags();
				uint8_t carryMask = bit3 ? 0x1 : 0x80;
				uint8_t rotateInMask = bit3 ? 0x80 : 0x1;
				uint8_t regVal = readValue(reg);
				setFlag(Flag::C, regVal & carryMask);
				if(bit3)
					regVal >>= 1;
				else
					regVal <<= 1;
				if(hadCarry) {
					regVal |= rotateInMask;
				}
				setFlag(Flag::Z, regVal == 0);
				writeValue(reg, regVal);
			}
				break;
			case Opcode_Prefix_Misc1_Command_Groups::SHIFT:
			{
				uint8_t val = readValue(reg);
				clearFlags();
				if(bit3) // Shift right
				{
					setFlag(Flag::C, val & 0x01);
					val >>= 1;
					if(val & 0x40) // Prevent MSB from changing
						val |= 0x80;
				}
				else
				{
					setFlag(Flag::C, val & 0x80);
					val <<= 1;
				}
				writeValue(reg, val);
				setFlag(Flag::Z, val);
			}
				break;
			case Opcode_Prefix_Misc1_Command_Groups::SWAP_SHIFT: // Swap/Shift Right, clear MSB
			{
				clearFlags();
				uint8_t val = readValue(reg);
				if(bit3) // Shift right
				{
					setFlag(Flag::C, val & 0x01);
					val >>= 1;
				}
				else //swap
				{
					val = (val >> 4) | (val << 4);
				}
				writeValue(reg, val);
				setFlag(Flag::Z, val);
			}
				break;
			}
		}
			break;
		case Opcode_Prefix_Group::TEST_BIT:
		{
			uint8_t bit = (instruction >> 3) & 0x3;
			setFlag(Flag::Z, readValue(reg) & (1 << bit));
			setFlag(Flag::H);
			clearFlag(Flag::N);
		}
			break;
		case Opcode_Prefix_Group::CLEAR_BIT:
		{
			uint8_t bit = (instruction >> 3) & 0x3;
			writeValue(reg, readValue(reg) & ~(1 << bit));
		}
			break;
		case Opcode_Prefix_Group::SET_BIT:
		{
			uint8_t bit = (instruction >> 3) & 0x3;
			writeValue(reg, readValue(reg) & (1 << bit));
		}
			break;
		}
	}
	uint8_t VM::addAndCalcCarry(uint8_t a, uint8_t b)
	{
		uint8_t ret = a + b;
		uint8_t carry = a ^ b ^ ret; // bit n is set if carry from bit n-1
		setFlag(Flag::H, carry & 0x10);
		setFlag(Flag::C, a > 0xFF - b);
		return ret;
	}

	uint16_t VM::addAndCalcCarry(uint16_t a, uint16_t b)
	{
		uint16_t ret = a + b;
		uint16_t carry = a ^ b ^ ret;
		setFlag(Flag::H, carry & 0x1000);
		setFlag(Flag::C, a > 0xFFFF - b);
		return ret;
	}
}
