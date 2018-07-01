#pragma once

#include <cstdint>
#include <vector>
/**
 * This file contains the memory bank controllers which intercept
 * attempts to write to certain areas of memory and perform bank
 * switching with the cartridge memory
 */

namespace gb_emu
{
	/**
	 * This is the base MBC
	 */
	class MBC
	{
	public:
		virtual void captureWrite(uint16_t addr, uint8_t byte, const std::vector<uint8_t>& cartridgeROM, uint8_t* memory) = 0;
		virtual ~MBC() {}
	};

	/**
	* This is the null MBC that just does nothing
	*/
	class MBC_Null : public MBC
	{
	public:
		virtual void captureWrite(uint16_t addr, uint8_t byte, const std::vector<uint8_t>& cartridgeROM, uint8_t* memory) override;
		~MBC_Null() = default;
	};

	class MBC1 : public MBC
	{
	public:
		virtual void captureWrite(uint16_t addr, uint8_t byte, const std::vector<uint8_t>& cartridgeROM, uint8_t* memory) override;
		~MBC1() = default;
	private:
		bool ROMBanking = true;
	};
}