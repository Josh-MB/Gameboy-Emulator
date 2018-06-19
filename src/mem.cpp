#include "..\include\mem.hpp"
#include <filesystem>
#include <cstdio>
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
		if(size > 0xFFFF) {
			fprintf(stderr, "File too big for GB memory: %s\n", path.c_str());
			return;
		}

		std::FILE* fp = std::fopen(path.c_str(), "rb");
		uint8_t tempMem[0xFFFF];

		try {
			std::fread(&(tempMem[0]), sizeof(tempMem[0]), static_cast<size_t>(size), fp);
			std::memcpy(&(memory[0]), &(tempMem[0]), MEM_SIZE);
		}
		catch(std::exception &e) {
			fprintf(stderr, "Error reading file: %s with error %s\n", path.c_str(), e.what());
			return;
		}
	}
}
