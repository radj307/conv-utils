#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <str.hpp>
#include <indentor.hpp>

namespace base {
	using value = long long;
	using otherval = std::string;


	inline value binaryToDecimal(const std::string& binary) noexcept(false)
	{
		value v{ 0ll };
		for (const auto& ch : binary) {
			v <<= 1ll;
			if (ch == '1')
				v |= 0b1;
			else if (ch != '0')
				throw make_exception("Invalid binary number: \'", ch, '\'');
		}
		return v;
	}
	inline otherval decimalToBinary(value n) noexcept
	{
		otherval v{};
		for (; n > 0ll; n /= 2ll)
			v += n % 2ll;
		return v;
	}

	/**
	 * @brief Converts a given character to its equivalent hexadecimal value.
	 * @param ch	- Input character. 1-9, A=10, B=11, C=12, D=13, E=14, F=15. Any character outside of this range will throw an exception.
	 * @returns value
	 */
	inline value hexToDecimal(const char ch) noexcept(false)
	{
		if (isdigit(ch))
			return static_cast<value>(ch - '0');
		else if (isalpha(ch))
			return (static_cast<value>(str::toupper(ch) - 'A') + 10);
		throw make_exception("getHexValue()\tFailed to convert \'", ch, "\' to hexadecimal!");
	}

	/**
	 * @brief			Convert from hexadecimal to decimal.
	 * @param hex		Input hexadecimal value.
	 * @returns			value
	 */
	inline value hexToDecimal(otherval const& hex) noexcept(false)
	{
		const auto from_base{ 16 };
		if (!hex.empty()) {
			value power{ 1 }, result{ 0 };
			for (auto c{ hex.rbegin() }; c != hex.rend(); ++c) {
				if (*c == 'x')
					break; // avoid checking "0x" prefix
				const auto v{ hexToDecimal(*c) };
				if (v >= from_base)
					throw make_exception("Hexadecimal value \'", *c, "\' converted to invalid hex number \"", v, "\"!\n", indent(10), "Please report this error to the developer!");
				result += v * power;
				power *= from_base;
			}
			return result;
		}
		else throw make_exception("Received an empty hexadecimal number, conversion failed!");
	}

	/**
	 * @brief			Convert from decimal to hexadecimal.
	 * @param dec		Input decimal value.
	 * @param ...fmt	Additional stream formatting objects to include before printing the value.
	 * @returns			otherval
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline otherval decimalToHex(value const& dec, Ts&&... fmt) noexcept
	{
		return str::stringify(
			std::hex,
			std::forward<Ts>(fmt)...,
			dec
		);
	}

	/**
	 * @brief			Convert from decimal to hexadecimal.
	 * @param dec		Input decimal value as a string.
	 * @param ...fmt	Additional stream formatting objects to include before printing the value.
	 * @returns			otherval
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline otherval decimalToHex(std::string const& dec, Ts&&... fmt) noexcept
	{
		return str::stringify(
			std::hex,
			std::forward<Ts>(fmt)...,
			str::stoll(dec)
		);
	}

	/**
	 * @enum ValueBase
	 * @brief Used to represent the base of a number. Only used internally within the base namespace.
	 */
	enum class ValueBase : unsigned char {
		INVALID,
		BINARY,
		OCTAL,
		DECIMAL,
		HEXADECIMAL,
	};

	static bool validBinary(const std::string& str)
	{
		const auto& lc{ str::tolower(str) };
		return std::all_of(str.begin() + (str::startsWith(lc, "0b") ? 2ull : 0ull), str.end(), [](auto&& c) {
			return c == '0' || c == '1';
		});
	}
	static bool validOctal(std::string str)
	{
		str = str::tolower(str);
		return std::all_of(str.begin() + !!str::startsWith(str, '\\'), str.end(), [](auto&& c) {
			return c >= '0' && c <= '7';
		});
	}
	static bool validDecimal(std::string str)
	{
		return std::all_of(str.begin() + !!str::startsWith(str, '-'), str.end(), [](auto&& c) {
			return isdigit(c) || c == '.';
		});
	}
	static bool validHexadecimal(const std::string& str)
	{
		const auto& lc{ str::tolower(str) };
		return std::all_of(lc.begin() + (str::startsWith(lc, "0x") ? 2ull : (str.size() > 0ull && str.at(0ull) == '#' ? 1ull : 0ull)), lc.end(), [](auto&& c) {
			return isdigit(c) || (c >= 'a' && c <= 'f');
		});
	}

	base::ValueBase operator|(const base::ValueBase& l, const base::ValueBase r)
	{
		return static_cast<base::ValueBase>(static_cast<unsigned char>(l) | static_cast<unsigned char>(r));
	}
	base::ValueBase operator&(const base::ValueBase& l, const base::ValueBase r)
	{
		return static_cast<base::ValueBase>(static_cast<unsigned char>(l) & static_cast<unsigned char>(r));
	}
	base::ValueBase operator^(const base::ValueBase& l, const base::ValueBase r)
	{
		return static_cast<base::ValueBase>(static_cast<unsigned char>(l) ^ static_cast<unsigned char>(r));
	}

	base::ValueBase& operator|=(base::ValueBase& l, const base::ValueBase r)
	{
		return l = static_cast<base::ValueBase>(static_cast<unsigned char>(l) | static_cast<unsigned char>(r));
	}
	base::ValueBase& operator&=(base::ValueBase& l, const base::ValueBase r)
	{
		return l = static_cast<base::ValueBase>(static_cast<unsigned char>(l) & static_cast<unsigned char>(r));
	}
	base::ValueBase& operator^=(base::ValueBase& l, const base::ValueBase r)
	{
		return l = static_cast<base::ValueBase>(static_cast<unsigned char>(l) ^ static_cast<unsigned char>(r));
	}

	template<var::numeric T> [[nodiscard]] bool operator==(const base::ValueBase& l, const T& r)
	{
		return static_cast<T>(l) == r;
	}
	template<var::numeric T> [[nodiscard]] bool operator!=(const base::ValueBase& l, const T& r)
	{
		return static_cast<T>(l) != r;
	}


	inline ValueBase detect_base(const std::string& arg, const ValueBase& allowedTypes)
	{
		if ((allowedTypes & ValueBase::BINARY) != 0 && validBinary(arg))
			return ValueBase::BINARY;
		else if ((allowedTypes & ValueBase::OCTAL) != 0 && validOctal(arg))
			return ValueBase::OCTAL;
		else if ((allowedTypes & ValueBase::DECIMAL) != 0 && validDecimal(arg))
			return ValueBase::DECIMAL;
		else if ((allowedTypes & ValueBase::HEXADECIMAL) != 0 && validHexadecimal(arg))
			return ValueBase::HEXADECIMAL;
		else return ValueBase::INVALID;
	}

}

using base::operator&;
using base::operator&=;
using base::operator|;
using base::operator|=;
using base::operator^;
using base::operator^=;
using base::operator==;
using base::operator!=;
