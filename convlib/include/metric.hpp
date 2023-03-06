/**
 * @file	metric.hpp
 * @author	radj307
 * @brief	Contains functions & types for the Metric system.
 */
#pragma once
#include <str.hpp>

#include <math.hpp>

namespace metric {
	enum class Prefix : std::int8_t {
		QUECTO = -30,
		RONTO = -27,
		YOCTO = -24,
		ZEPTO = -21,
		ATTO = -18,
		FEMTO = -15,
		PICO = -12,
		NANO = -9,
		MICRO = -6,
		MILLI = -3,
		CENTI = -2,
		DECI = -1,
		BASE = 0,
		DECA = 1,
		HECTO = 2,
		KILO = 3,
		MEGA = 6,
		GIGA = 9,
		TERA = 12,
		PETA = 15,
		EXA = 18,
		ZETTA = 21,
		YOTTA = 24,
		RONNA = 27,
		QUETTA = 30,
	};

	/**
	 * @brief		Parse a string to identify metric prefix names. (Ex: "milli-", "centi-", "kilo-", etc.)
	 * @param str	Input String. This is automatically converted to lowercase.
	 * @returns		Prefix
	 */
	inline Prefix parseMetricPrefixName(std::string str)
	{
		str = str::tolower(str);
		if (str.size() >= 3ull) {
			if (str::startsWith(str, "exa"))
				return Prefix::EXA;

			if (str.size() >= 4ull) {
				if (str::startsWith(str, "atto"))
					return Prefix::ATTO;
				if (str::startsWith(str, "pico"))
					return Prefix::PICO;
				if (str::startsWith(str, "nano"))
					return Prefix::NANO;
				if (str::startsWith(str, "deci"))
					return Prefix::DECI;
				if (str::startsWith(str, "deca"))
					return Prefix::DECA;
				if (str::startsWith(str, "kilo"))
					return Prefix::KILO;
				if (str::startsWith(str, "mega"))
					return Prefix::MEGA;
				if (str::startsWith(str, "giga"))
					return Prefix::GIGA;
				if (str::startsWith(str, "tera"))
					return Prefix::TERA;
				if (str::startsWith(str, "peta"))
					return Prefix::PETA;

				if (str.size() >= 5ull) {
					if (str::startsWith(str, "ronto"))
						return Prefix::RONTO;
					if (str::startsWith(str, "quecto"))
						return Prefix::QUECTO;
					if (str::startsWith(str, "yocto"))
						return Prefix::YOCTO;
					if (str::startsWith(str, "zepto"))
						return Prefix::ZEPTO;
					if (str::startsWith(str, "femto"))
						return Prefix::FEMTO;
					if (str::startsWith(str, "micro"))
						return Prefix::MICRO;
					if (str::startsWith(str, "milli"))
						return Prefix::MILLI;
					if (str::startsWith(str, "centi"))
						return Prefix::CENTI;
					if (str::startsWith(str, "hecto"))
						return Prefix::HECTO;
					if (str::startsWith(str, "zetta"))
						return Prefix::ZETTA;
					if (str::startsWith(str, "yotta"))
						return Prefix::YOTTA;
					if (str::startsWith(str, "ronna"))
						return Prefix::RONNA;
					if (str::startsWith(str, "quetta"))
						return Prefix::QUETTA;
				}
			}
		}
		return Prefix::BASE;
	}
	/**
	 * @brief		Parse a string to identify metric prefix symbols. (Ex: "n-", "�-", "m-", "k-", "M-", "G-", etc.)
	 * @param str	Input String.
	 * @returns		Prefix
	 */
	inline Prefix parseMetricPrefixSymbol(std::string str)
	{
		if (str.size() >= 1ull) {
			switch (str.at(0ull)) {
			case 'q':
				return Prefix::QUECTO;
			case 'r':
				return Prefix::RONTO;
			case 'y':
				return Prefix::YOCTO;
			case 'z':
				return Prefix::ZEPTO;
			case 'a':
				return Prefix::ATTO;
			case 'f':
				return Prefix::FEMTO;
			case 'p':
				return Prefix::PICO;
			case 'n':
				return Prefix::NANO;
			case 'u':// [[fallthrough]];
			//case 'µ':
				return Prefix::MICRO;
			case 'm':
				return Prefix::MILLI;
			case 'c':
				return Prefix::CENTI;
			case 'd':
				if (str.size() >= 2ull && str.at(1ull) != 'a')
					return Prefix::DECI;
				else return Prefix::DECA;
			case 'h':
				return Prefix::HECTO;
			case 'k':
				return Prefix::KILO;
			case 'M':
				return Prefix::MEGA;
			case 'G':
				return Prefix::GIGA;
			case 'T':
				return Prefix::TERA;
			case 'P':
				return Prefix::PETA;
			case 'E':
				return Prefix::EXA;
			case 'Z':
				return Prefix::ZETTA;
			case 'Y':
				return Prefix::YOTTA;
			case 'R':
				return Prefix::RONNA;
			case 'Q':
				return Prefix::QUETTA;
			default:
				return Prefix::BASE;
			}
		}
	}

	using NumberType = long double;

	/**
	 * @brief		Retrieve the conversion factor for a given prefix.
	 * @param pwr	A Metric Unit Power.
	 * @returns		NumberType
	 */
	inline NumberType getConversionFactor(Prefix const& pwr)
	{
		return std::pow(10.0L, static_cast<NumberType>(pwr));
	}

	/**
	 * @struct	UnitBase
	 * @brief	Contains a Prefix & conversion factor used to identify & convert between units.
	 */
	struct UnitBase {
		// @brief	Conversion Factor. This is the result of (10 ^ (POWER))
		NumberType cf;
		Prefix prefix;

		UnitBase(Prefix const& prefixType) : cf{ getConversionFactor(prefixType) }, prefix{ prefixType } {}
		virtual ~UnitBase() = default;
	};

	struct UnitRepresentation {
		std::string _sym, _name;
		bool useSymbol;

		UnitRepresentation(const std::string& symbol, const std::string& name) : _sym{ symbol }, _name{ name } {}

		friend std::ostream& operator<<(std::ostream& os, const UnitRepresentation& u)
		{
			if (u.useSymbol)
				os << u._sym;
			else
				os << u._name;
			return os;
		}
	};
}