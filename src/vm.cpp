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
					Opcode_Register r1 = static_cast<Opcode_Register>(instruction | 0x38);
					Opcode_Register r2 = static_cast<Opcode_Register>(instruction | 0x07);
					uint8_t value;
					if(r2 == Opcode_Register::HL) {
						value = getByte(registerPairs[toUType(RegisterPair::HL)]);
					}
					else {
						value = getRegister(getRegister_from_OpcodeRegister(r2));
					}

					if(r1 == Opcode_Register::HL) {
						setByte(registerPairs[toUType(RegisterPair::HL)], value);
					}
					else {
						setRegister(getRegister_from_OpcodeRegister(r1), value);
					}
				}
			}
				break;
			case Opcode_Group::ARITH:
				break;
			case Opcode_Group::MISC2:
				break;
			default:
				return ExecuteResult::RUNTIME_ERROR;
			}
		}
		return ExecuteResult();
	}
}
