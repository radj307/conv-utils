#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <strconv.hpp>

namespace hex {

	template<class RT>
	static std::enable_if_t<std::is_integral_v<RT>, RT> hex_to_int(const std::string& hex)
	{
		std::stringstream ss;
		ss << std::hex << hex;
		RT val{ static_cast<RT>(0) };
		hex >> val;
		return val;
	}

	template<class T>
	static std::enable_if_t<std::is_integral_v<T>, std::string> int_to_hex(const T& number, const bool uppercase = true)
	{
		std::stringstream ss;
		ss << std::hex << (uppercase ? std::uppercase : std::nouppercase) << number;
		return ss.str();
	}

	template<class InType>
	struct ToHex {
		InType _number;
		bool _uppercase;

		explicit ToHex(InType n, bool uppercase = true) : _number{ std::move(n) }, _uppercase{ std::move(uppercase) }
		{
			static_assert(std::is_integral_v<InType> || std::is_floating_point_v<InType>, "InType must be numeric!");
		}

		explicit operator InType() const { return _number; }

		friend std::ostream& operator<<(std::ostream& os, const ToHex<InType>& obj)
		{
			static_assert(std::is_integral_v<InType>, "Cannot convert non-number to hex!");
			os << int_to_hex(obj._number, obj._uppercase);
			return os;
		}
	};

	/**
	 * @brief Check whether a given string contains a valid hexadecimal value.
	 * @param str				- String to check.
	 * @param require_letter	- When true, only returns true if there is at least one valid letter in the number, when false, numbers will still be considered hexadecimal if they have only numbers.
	 */
	inline bool is_hex_num(const std::string& str, const bool require_letter = true)
	{
		bool has_letter{ false };
		return (str.size() >= 2 && *str.begin() == '0' && *(str.begin() + 1u) == 'x') || std::all_of(str.begin(), str.end(), [&has_letter](auto&& ch) {
			if (isdigit(ch))
				return true;
			const auto lc{ std::tolower(ch) };
			if (lc >= 'a' && lc <= 'f') {
				has_letter = true;
				return true;
			}
			return false;
			}) && ((bool)((int)(has_letter) & (int)(require_letter)));// ( ( !has_letter && !require_letter ) || ( has_letter && require_letter ) );
	}

	/**
	 * @brief Converts a hexadecimal string to unsigned integer.
	 * @param hex	- A string containing only a hexadecimal number.
	 * @returns unsigned
	 */
	inline unsigned hex_to_int(const std::string& hex)
	{
		std::stringstream ss;
		ss << std::hex << hex; // set the hex fmtflag and insert the hex string
		unsigned num{ 0u }; // create var to hold result
		ss >> num; // use stringstream to cast hex to unsigned int
		return num;
	}
	inline unsigned hex_to_int(const char* hex_c_str)
	{
		return hex_to_int(std::string(hex_c_str));
	}

	template<class InType>
	struct ToDec {
		InType _altnumber;

		explicit ToDec(InType n) : _altnumber{ std::move(n) }
		{
			static_assert(std::is_same_v<InType, std::string> || std::is_same_v<InType, char*>);
		}

		explicit operator InType() const { return _altnumber; }

		friend std::ostream& operator<<(std::ostream& os, const ToDec<InType>& obj)
		{
			os << hex_to_int(obj._altnumber);
			return os;
		}
	};
}