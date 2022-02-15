#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <str.hpp>
#include <indentor.hpp>

namespace base {
	/**
	 * @brief Converts a given character to its equivalent hexadecimal value.
	 * @param ch	- Input character. 1-9, A=10, B=11, C=12, D=13, E=14, F=15. Any character outside of this range will throw an exception.
	 * @returns int
	 */
	inline int getHexValue(const char ch) noexcept(false)
	{
		if (isdigit(ch))
			return static_cast<int>(ch - '0');
		else if (isalpha(ch))
			return (static_cast<int>(str::toupper(ch) - 'A') + 10);
		throw make_exception("getHexValue()\tFailed to convert \'", ch, "\' to hexadecimal!");
	}

	using decval = long long;
	using hexval = std::string;

	/**
	 * @brief			Convert from hexadecimal to decimal.
	 * @param hex		Input hexadecimal value.
	 * @returns			decval
	 */
	inline decval to_decimal(hexval const& hex) noexcept(false)
	{
		const auto from_base{ 16 };
		if (!hex.empty()) {
			int power{ 1 }, result{ 0 };
			for (auto c{ hex.rbegin() }; c != hex.rend(); ++c) {
				if (*c == 'x')
					break; // avoid checking "0x" prefix
				const auto v{ getHexValue(*c) };
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
	 * @returns			hexval
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline hexval to_hex(decval const& dec, Ts&&... fmt) noexcept
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
	 * @returns			hexval
	 */
	template<var::Streamable<std::stringstream>... Ts>
	inline hexval to_hex(std::string const& dec, Ts&&... fmt) noexcept
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
	enum class ValueBase {
		INVALID,
		DECIMAL,
		HEXADECIMAL,
	};

	inline constexpr bool is_hex_char(const char& ch)
	{
		const auto upper{ str::toupper(ch) };
		return upper >= 'A' && upper <= 'F';
	}

	inline ValueBase detect_base(const std::string& arg)
	{
		if (arg.find("0x") == 0ull || std::any_of(arg.begin(), arg.end(), is_hex_char))
			return ValueBase::HEXADECIMAL;
		else if (std::all_of(arg.begin(), arg.end(), [](auto&& ch) { return isdigit(ch) || ch == '.' || ch == ','; }))
			return ValueBase::DECIMAL;
		return ValueBase::INVALID;
	}
}