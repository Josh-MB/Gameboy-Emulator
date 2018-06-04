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
				Opcode_Register r1 = static_cast<Opcode_Register>(instruction | 0x38);
				Opcode_Register r2 = static_cast<Opcode_Register>(instruction | 0x07);
				if(r1 == Opcode_Register::HL && r2 == Opcode_Register::HL) {
					// Halt
				}
				else {
					uint8_t value;
					if(r2 == Opcode_Register::HL) {
						value = getByte(registerPairs[toUType(RegisterPair::HL)]);
					}
					else if(r2 == Opcode_Register::A) {
						value = getRegister(Register::A);
					}
					else {
						value = getRegister(static_cast<Register>(r2));
					}

					if(r1 == Opcode_Register::HL) {
						setByte(registerPairs[toUType(RegisterPair::HL)], value);
					}
					else if(r2 == Opcode_Register::A) {
						setRegister(Register::A, value);
					}
					else {
						setRegister(static_cast<Register>(r1), value);
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
