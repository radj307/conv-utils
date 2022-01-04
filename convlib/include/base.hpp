#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <str.hpp>
#include <OutputHelper.hpp>

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
		throw std::exception(str::stringify("getHexValue()\tFailed to convert \'", ch, "\' to hexadecimal!").c_str());
	}

	/**
	 * @brief Convert a number in a base larger than 10 to decimal.
	 * @param hex		- Input number as a string. This is currently only set up to allow hexadecimal input.
	 * @param from_base - Number of distinct values per digit position.
	 * @returns int
	 */
	inline int to_decimal(std::string hex) noexcept(false)
	{
		const auto from_base{ 16 };
		bool is_negative{ hex.front() == '-' };
		if (is_negative)
			hex = hex.substr(1ull);
		if (!hex.empty()) {
			int power{ 1 }, result{ 0 };
			for (auto ch{ hex.rbegin() }; ch != hex.rend(); ++ch) {
				if (*ch == 'x')
					break;
				const auto val{ getHexValue(*ch) };
				if (val >= from_base)
					throw std::exception(std::string("to_decimal()\tFailed to convert \"" + hex + "\" Received invalid value \'" + std::to_string(val) + "\' from getHexValue()!").c_str());
				result += val * power;
				power = power * from_base;
			}
			return is_negative ? -result : result;
		}
		return -1;
	}

	inline std::string to_hex(auto value) noexcept(false)
	{
		const bool is_negative{ value < 0 };
		return str::stringify(std::hex, std::uppercase, (OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder), (is_negative ? "-0x" : (OutputSettings.hide_types ? "" : "0x")));
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