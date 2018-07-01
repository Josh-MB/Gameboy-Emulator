#include "..\include\mem.hpp"
#include "..\include\reservedAddresses.hpp"
#include <filesystem>
#include <cstdio>
#include <gsl/gsl_util>
namespace fs = std::filesystem;

namespace gb_emu
{
	void MMU::loadFromFile(std::string path)
	{
		fs::path p = path;

		if(!fs::exists(p)) {
			fprintf(stderr, "Trying to load non-existent file: %s\n", path.c_str());
			return;
		}
		auto size = fs::file_size(p);
		// 8 MB is the hard cap for a cartridge ROM, I believe.
		// At least 4 MB is needed for all possible switchable ROM banks
		if(size > MAX_CARTRIDGE_SIZE) {
			fprintf(stderr, "File too big for GB cartridge: %s\n", path.c_str());
			return;
		}

		std::FILE* fp = std::fopen(path.c_str(), "rb");

		// Make sure file handle gets closed regardless of exceptions
		auto cleanup = gsl::finally([&] {std::fclose(fp); });

		try {
			// We know from above that size can't be bigger than MAX_CARTRIDGE_SIZE
			// so this narrowing cast is fine
			size_t sz = static_cast<size_t>(size);
			cartridgeROM.resize(sz);
			std::fread(&(cartridgeROM[0]), sizeof(cartridgeROM[0]), sz, fp);

			// Copy the first ROM block over to system ram
			std::memcpy(&(memory[0]), &(cartridgeROM[0]), ROM_BLOCK_SIZE);
		}
		catch(std::exception &e) {
			fprintf(stderr, "Error reading file: %s with error %s\n", path.c_str(), e.what());
			return;
		}
	}
	uint8_t MMU::getByte(uint16_t addr) const
	{
		return memory[addr];
	}

	void  MMU::setByte(uint16_t addr, uint8_t value)
	{
		// Perform echo writes
		if(addr >= WORKING_RAM_BANK && addr <= WORKING_RAM_BANK_ECHO_END) {
			memory[addr + ECHO_OFFSET] = value;
		}
		if(addr >= ECHO_RAM_BANK && addr <= ECHO_RAM_BANK_END) {
			memory[addr - ECHO_OFFSET] = value;
		}

		memory[addr] = value;
	}
}
