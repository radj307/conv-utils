#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <strconv.hpp>
#include <OutputHelper.hpp>

namespace base {
	inline int getHexValue(const char ch)
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
	inline int to_decimal(std::string hex, int from_base = 16)
	{
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
			return result;
		}
		return -1;
	}

	struct Conversion {
		using VariantT = std::variant<std::monostate, int, std::string>;
		VariantT _in, _out;
		Conversion(const std::string& input) : _in{ input }, _out{ to_decimal(input) } {}
	};
}