#include "..\include\mbc.hpp"
#include "..\include\reservedAddresses.hpp"

namespace gb_emu
{
	void MBC_Null::captureWrite(uint16_t addr, uint8_t byte, const std::vector<uint8_t>& cartridgeROM, uint8_t * memory)
	{
		/** Do nothing */
	}

	void MBC1::captureWrite(uint16_t addr, uint8_t byte, const std::vector<uint8_t>& cartridgeROM, uint8_t * memory)
	{
		// Select a ROM bank to swap in
		if(addr >= 0x2000 && addr <= 0x3FFF) {
			uint8_t romBank = byte & 0x1F;
			// RomBanks 0x00, 0x20, 0x40, 0x60 -> 0x01, 0x21, 0x41, 0x61
			if(romBank == 0) romBank += 1;
			//Where are rom blocks stored? I think from address 0?
			std::memcpy(&(memory[0]), &(cartridgeROM[romBank * ROM_BLOCK_SIZE]), ROM_BLOCK_SIZE);

		}
	}
}