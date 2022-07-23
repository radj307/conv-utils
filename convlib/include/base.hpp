#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <str.hpp>
#include <indentor.hpp>
#include <strmath.hpp>

namespace base {
#	pragma region BaseEnum

	namespace enumerator {
		// @brief	The underlying type of the Base enum.
		using BaseT = unsigned char;
		// @brief	Enum that represents recognized numeric bases.
		enum class Base : BaseT {
			ZERO = 0,
			BINARY = 2,
			OCTAL = 8,
			DECIMAL = 10,
			HEXADECIMAL = 16,
		};
		[[nodiscard]] Base operator|(const Base& l, const Base& r) { return static_cast<Base>(static_cast<BaseT>(l) | static_cast<BaseT>(r)); }
		[[nodiscard]] Base operator^(const Base& l, const Base& r) { return static_cast<Base>(static_cast<BaseT>(l) ^ static_cast<BaseT>(r)); }
		[[nodiscard]] Base operator&(const Base& l, const Base& r) { return static_cast<Base>(static_cast<BaseT>(l) & static_cast<BaseT>(r)); }
		[[nodiscard]] bool operator==(Base& base, const BaseT& o) { return static_cast<BaseT>(base) == o; }
		[[nodiscard]] bool operator==(BaseT& baset, const Base& o) { return baset == static_cast<BaseT>(o); }
		[[nodiscard]] bool operator!=(Base& base, auto&& o) { return !operator==(base, std::forward<decltype(o)>(o)); }

		inline std::string BaseToString(const Base& b)
		{
			switch (b) {
			case Base::BINARY:
				return "Binary";
			case Base::OCTAL:
				return "Octal";
			case Base::DECIMAL:
				return "Decimal";
			case Base::HEXADECIMAL:
				return "Hexadecimal";
			case Base::ZERO: [[fallthrough]];
			default:
				break;
			}
			return "(null)";
		}
		inline Base StringToBase(std::string str)
		{
			str = str::tolower(str);

			if (str == "binary")
				return Base::BINARY;
			else if (str == "octal")
				return Base::OCTAL;
			else if (str == "decimal")
				return Base::DECIMAL;
			else if (str == "hexadecimal")
				return Base::HEXADECIMAL;

			return Base::ZERO;
		}
	}
	using enumerator::Base;

#	pragma endregion BaseEnum

	inline WINCONSTEXPR Base detectBase(const std::string& s, const Base& allowBases = Base::BINARY | Base::OCTAL | Base::DECIMAL | Base::HEXADECIMAL)
	{
		const auto& allow{ [&allowBases](const Base& base) {
			return (base & allowBases) != Base::ZERO;
		} };
		// check string prefix:
		if (str::startsWith(s, "0b"))
			return Base::BINARY;
		else if (str::startsWith(s, '\\'))
			return Base::OCTAL;
		else if (str::startsWith(s, "0x"))
			return Base::HEXADECIMAL;
		// fallback to checking string contents:
		// don't check binary or octal because both must be explicitly specified and both contain all decimal digits.
		else if (str::isdecimal(s))
			return Base::DECIMAL;
		else if (str::ishex(s))
			return Base::HEXADECIMAL;
		// invalid number:
		return Base::ZERO;
	}
}

using base::enumerator::Base;
using base::enumerator::operator|;
using base::enumerator::operator^;
using base::enumerator::operator&;
using base::enumerator::operator==;
using base::enumerator::operator!=;
using base::enumerator::BaseToString;
using base::enumerator::StringToBase;