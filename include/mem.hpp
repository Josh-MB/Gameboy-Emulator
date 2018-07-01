#pragma once

#include "common.hpp"
#include "reservedAddresses.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace gb_emu
{
	class MBC;

	class MMU
	{
	private:
		uint8_t memory[MEM_SIZE];

		std::vector<uint8_t> cartridgeROM;

		MBC* mbc = nullptr;

		void clear();
	public:
		~MMU();
		void loadFromFile(std::string path);
		inline uint8_t getZeroPageByte(uint8_t addr) const { return memory[0xFF00 + addr]; }
		inline void setZeroPageByte(uint8_t addr, uint8_t value) { memory[0xFF00 + addr] = value; }


		uint8_t getByte(uint16_t addr) const;
		void setByte(uint16_t addr, uint8_t value);

		/**
		* Fetches the next double byte, and increments program counter
		* twice. Assumes LSB is stored at addr
		*/
		inline uint16_t getDouble(uint16_t addr) const { return memory[addr] | (memory[addr + 1] << 8); };

		/**
		* Write double to memory, with LSB first
		*/
		inline void setDouble(uint16_t addr, uint16_t value) {
			memory[addr] = static_cast<uint8_t>(value & 0xFF);
			memory[addr + 1] = static_cast<uint8_t>(value >> 8);
		}


	};
}