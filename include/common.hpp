#pragma once

#include <type_traits>

#define GB_DEBUG true

namespace gb_emu
{
	template<typename E, typename = std::enable_if_t<std::is_enum<E>::value > >
	constexpr auto
		toUType(E enumerator) noexcept {
		return static_cast<std::underlying_type_t<E>>(enumerator);
	}
}