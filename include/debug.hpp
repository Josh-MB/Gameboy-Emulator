#pragma once

#include "common.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstdint>

namespace gb_emu
{
	enum class Register : uint8_t;
	void check_register(Register r);

#ifdef _DEBUG
#define CHECK_REGISTER(r) (check_register(r))
#else
#define CHECK_REGISTER(r) (sizeof(r))
#endif


}