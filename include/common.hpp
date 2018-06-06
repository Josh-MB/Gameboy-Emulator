#pragma once

#include <type_traits>
#include <cstdint>

namespace gb_emu
{
	/**
	 * Converts enum to underlying type
	 */
	template<typename E, typename = std::enable_if_t<std::is_enum<E>::value > >
	constexpr auto
		toUType(E enumerator) noexcept {
		return static_cast<std::underlying_type_t<E>>(enumerator);
	}

	/**
	 * Constructs enum using underlying type and enum member E::MASK
	 */
	template<typename E, typename = std::enable_if_t<std::is_enum<E>::value> >
	constexpr E
		toEnum(std::underlying_type_t<E> val) noexcept {
		return static_cast<E>(val & static_cast<std::underlying_type_t<E>>(E::MASK));
	}
}