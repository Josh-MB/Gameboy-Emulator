#pragma once

/**
 * This file sets constants for all the reserved memory addresses
 */

#include <cstdint>

namespace gb_emu
{
	enum ReservedAddresses : uint16_t {
		INTERRUPT_FLAG = 0xFF0F,
		INTERRUPT_ENABLE = 0xFFFF,
	};
}