#include "../include/debug.hpp"
#include "../include/vm.hpp"

namespace gb_emu {
	void check_register(Register r) {
		if(r == Register::HL_UNUSED_UPPER || r == Register::HL_UNUSED_LOWER)
		{
			fprintf(stderr, "Trying to access fake HL register\n");
			std::exit(EXIT_FAILURE);
		}
	}
}