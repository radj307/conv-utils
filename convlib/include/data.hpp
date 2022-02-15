#pragma once
#include <sysarch.h>
#include <strmanip.hpp>
#include <strconv.hpp>
#include <ParamsAPI2.hpp>

#include <utility>

namespace data {
	/**
	 * @struct Unit
	 * @brief Represents a digital data size unit, from bytes to yottabytes.
	 */
	struct Unit {
		const unsigned _index; ///< @brief Sequential index value assigned to this unit
		const std::string _sym; ///< @brief The plaintext symbol used to represent this unit

		WINCONSTEXPR Unit(unsigned index, std::string symbol) : _index{ std::move(index) }, _sym{ std::move(symbol) } {}
		WINCONSTEXPR Unit(const Unit& o) : _index{ o._index }, _sym{ o._sym } {}

		CONSTEXPR operator unsigned() const { return _index; }
		WINCONSTEXPR operator const std::string() const { return _sym; }

		bool operator==(const Unit& o) const { return _index == o._index; }
		bool operator!=(const Unit& o) const { return !this->operator==(o); }
		bool operator<(const Unit& o) const { return _index < o._index; }
		bool operator>(const Unit& o) const { return _index > o._index; }
		bool operator<=(const Unit& o) const { return _index <= o._index; }
		bool operator>=(const Unit& o) const { return _index >= o._index; }

		// Inserts the symbol of this unit into an output stream
		friend std::ostream& operator<<(std::ostream& os, const Unit& obj)
		{
			os << obj._sym;
			return os;
		}

		// declare unit types
		static const Unit
			UNKNOWN,
			BYTE,
			KILOBYTE,
			MEGABYTE,
			GIGABYTE,
			TERABYTE,
			PETABYTE,
			EXABYTE,
			ZETTABYTE,
			YOTTABYTE;
	};
	// define unit types
	inline const Unit
		Unit::UNKNOWN{ 0u, "?" },
		Unit::BYTE{ 1u, "B" },
		Unit::KILOBYTE{ 2u, "kB" },
		Unit::MEGABYTE{ 3u, "MB" },
		Unit::GIGABYTE{ 4u, "GB" },
		Unit::TERABYTE{ 5u, "TB" },
		Unit::PETABYTE{ 6u, "PB" },
		Unit::EXABYTE{ 7u, "EB" },
		Unit::ZETTABYTE{ 8u, "ZB" },
		Unit::YOTTABYTE{ 9u, "YB" };

	/**
	 * @brief Get a Unit from its associated index.
	 * @param index	- The index of the desired Unit.
	 * @returns Unit
	 * @throws std::exception	- Invalid index
	 */
	inline Unit get_unit_from_index(const unsigned index)
	{
		switch (index) {
		case 0u:
			return Unit::UNKNOWN;
		case 1u:
			return Unit::BYTE;
		case 2u:
			return Unit::KILOBYTE;
		case 3u:
			return Unit::MEGABYTE;
		case 4u:
			return Unit::GIGABYTE;
		case 5u:
			return Unit::TERABYTE;
		case 6u:
			return Unit::PETABYTE;
		case 7u:
			return Unit::EXABYTE;
		case 8u:
			return Unit::ZETTABYTE;
		case 9u:
			return Unit::YOTTABYTE;
		default:
			throw make_exception("data::get_unit_from_index()\tInvalid index : \'", index, "\'");
		}
	}

	/**
	 * @struct Size
	 * @brief Represents a size value, measured in binary Units.
	 */
	struct Size {
		const Unit _type;
		const long double _value;

		/**
		 * @brief Default Constructor
		 * @param size	- The Unit type used to measure this data size's value.
		 * @param value - The value of this data size.
		 */
		WINCONSTEXPR Size(Unit size, long double value) : _type{ std::move(size) }, _value{ std::move(value) } {}
		/// @brief Copy Constructor
		WINCONSTEXPR Size(const Size& o) : _type{ o._type }, _value{ o._value } {}

		bool operator==(const Size& o) const { return _type == o._type && _value == o._value; }
		bool operator!=(const Size& o) const { return !operator==(o); }

		/**
		 * @brief Return a copy of this value, converted to a given unit.
		 * @param size	- The size type to convert to.
		 * @returns Size
		 */
		Size convert_to(const Unit& size) const
		{
			// difference exponent = target size - my size
			const auto diff_exp{ static_cast<long double>(size.operator unsigned int()) - static_cast<long double>(_type.operator unsigned int()) };
			// divisor = 1024 ^ difference
			const auto div{ pow(1024.0, diff_exp) };

			if (div == 0.0)
				throw make_exception("Size::convert_to()\tCan't divide by zero!");

			// result = value / exponent
			return{ size, _value / div };
		}
	};

	/**
	 * @brief Determine which unit symbol a given string contains.
	 * @param str	- Input string.
	 * @returns Unit
	 */
	inline Unit determine_unit(std::string str, const bool whole_word_only = false)
	{
		str = str::tolower(str);

		const auto hasSymbol{ [&str, &whole_word_only](const Unit& sz) {
			return whole_word_only
				? (!sz._sym.empty() && str == str::tolower(sz._sym)) // must be case-insensitive match with no extra characters
				: (!sz._sym.empty() && str::pos_valid(str.find(str::tolower(sz._sym)))); // case-insensitive match anywhere in string
		} };

		// check kilobytes first
		if (hasSymbol(Unit::KILOBYTE))
			return Unit::KILOBYTE;
		// check megabytes
		if (hasSymbol(Unit::MEGABYTE))
			return Unit::MEGABYTE;
		// check gigabytes
		if (hasSymbol(Unit::GIGABYTE))
			return Unit::GIGABYTE;
		// check terabytes
		if (hasSymbol(Unit::TERABYTE))
			return Unit::TERABYTE;
		// check petabytes
		if (hasSymbol(Unit::PETABYTE))
			return Unit::PETABYTE;
		// check exabytes
		if (hasSymbol(Unit::EXABYTE))
			return Unit::EXABYTE;
		// check yottabytes
		if (hasSymbol(Unit::YOTTABYTE))
			return Unit::YOTTABYTE;
		// check bytes last, checking this first would always return bytes as all other units have a 'b'
		if (hasSymbol(Unit::BYTE))
			return Unit::BYTE;

		return Unit::UNKNOWN;
	}

	/**
	 * @brief Retrieve a Size from a string that contains both a numeric value, and alphabetic Unit symbol.
	 * @param str	- Input string.
	 * @returns Size
	 */
	inline Size str_to_size(std::string str)
	{
		const long double num{ [](std::string stringnum) {
			// remove all non-digits
			stringnum.erase(std::remove_if(stringnum.begin(), stringnum.end(), [](auto&& ch) { return !isdigit(ch); }), stringnum.end());
			return str::stod(stringnum);
		}(str) };
		return{ determine_unit(str), num };
	}

	struct ConvertUnit {
		const Size _in;
		const Unit _out;
		ConvertUnit(Size input, Unit out_unit) : _in{ std::move(input) }, _out{ std::move(out_unit) } {}

		operator Size() const { return _in.convert_to(_out); }
	};

	/**
	 * @struct Conversion
	 * @brief Handle a binary data size conversion, using argument vector iterators.
	 */
	struct Conversion {
		std::optional<std::unique_ptr<Size>> _in{ std::nullopt }, _out{ std::nullopt };

		/**
		 * @brief Default Constructor, converts a Size value to its equivalent value in another Unit.
		 * @param in	- A Size instance.
		 * @param out	- The Unit to convert to.
		 */
		Conversion(Size in, Unit out) : _in{ std::make_unique<Size>(std::move(in)) }, _out{ std::make_unique<Size>(std::move(_in.value().get()->convert_to(out))) } {}

		using IteratorT = std::vector<std::string>::const_iterator;
		/**
		 * @brief Advanced Constructor
		 * @param it
		 * @param end
		 */
		Conversion(IteratorT& it, const IteratorT& end)
		{
			const auto origin{ it };
			if (it != end - 1u) {
				if (const Unit here{ determine_unit(*it) }; here != Unit::UNKNOWN)
					_in = std::make_unique<Size>(Size{ here, str::stold(*++it)});
				// check if the input value & type are merged (ex: "512GB")
				else if (const auto fst_merged{ str_to_size(*origin) }; fst_merged._type != Unit::UNKNOWN)
					_in = std::make_unique<Size>(Size{ fst_merged._type, fst_merged._value });
				else // use the next argument as the input type (ex: "512 GB")
					_in = std::make_unique<Size>(Size{ determine_unit(*++it), str::stold(*origin) });

				if (it != end - 1u) { // if there is another argument in the list, use it as the output type
					if (const auto out_t{ determine_unit(*++it, true) }; out_t != Unit::UNKNOWN)
						_out = std::make_unique<Size>(_in.value().get()->convert_to(out_t));
					else
						it = origin;
				}
				else it = origin;
			}
		}
	};
}