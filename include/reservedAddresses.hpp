#pragma once

/**
 * This file sets constants for all the reserved memory addresses
 */

#include <cstdint>

namespace gb_emu
{
	enum ReservedAddresses : uint16_t {
		FIXED_ROM_BANK = 0x0000,

		/** Cartridge header info */
		CARTRIDGE_TYPE_FLAG = 0x0147,

		FIXED_ROM_BANK_END = 0x3FFF,
		SWITCHABLE_ROM_BANK = 0x4000,
		SWITCHABLE_ROM_BANK_END = 0x7FFF,
		VRAM_BANK = 0x8000,
		VRAM_BANK_END = 0x9FFF,
		EXTERNAL_RAM_BANK = 0xA000,
		EXTERNAL_RAM_BANK_END = 0xBFFF,
		WORKING_RAM_BANK = 0xC000,
		WORKING_RAM_BANK_END = 0xDFFF,
		ECHO_RAM_BANK = 0xE000,
		ECHO_RAM_BANK_END = 0xFDFF,
		ECHO_OFFSET = ECHO_RAM_BANK - WORKING_RAM_BANK,
		WORKING_RAM_BANK_ECHO_END = WORKING_RAM_BANK_END - ECHO_OFFSET,

		OAM_TABLE = 0xFE00,
		OAM_TABLE_END = 0xFE9F,

		/** I/O Registers */
		INTERRUPT_FLAG = 0xFF0F,


		HRAM = 0xFF80,
		HRAM_END = 0xFFFE,

		INTERRUPT_ENABLE = 0xFFFF,
	};

	// Address range 0x0000 to 0xFFFF
	constexpr size_t MEM_SIZE = 0x10000;
	constexpr size_t MAX_CARTRIDGE_SIZE = 0x800000;
	constexpr size_t ROM_BLOCK_SIZE = 0x4000;
}