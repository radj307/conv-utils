/**
 * @file	unit.hpp
 * @author	radj307
 * @brief	Contains real-world measurement unit converters, currently supporting metric & imperial.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <str.hpp>
#include <var.hpp>
#include <math.hpp>
#include <TermAPI.hpp>

#include <optional>
#include <iterator>
#include <algorithm>

namespace unit {
	/**
	 * @enum	SystemID
	 * @brief	Accepted Measurement Systems
	 */
	enum class SystemID : char {
		METRIC,
		IMPERIAL,
		ALL,
	};

	class Unit {
		SystemID _system;
		std::string _sym, _name;

	public:
		long double unitcf; // unit conversion factor

		WINCONSTEXPR Unit(SystemID const& system, long double const& unit_conversion_factor, std::string_view const& symbol, std::string_view const& full_name = {}) : _system{ system }, unitcf{ unit_conversion_factor }, _sym{ symbol }, _name{ full_name } {}

		/// @brief	Retrieve the given value in it's base form.
		CONSTEXPR long double to_base(const long double& val) const { return val * unitcf; }
		CONSTEXPR SystemID getSystem() const noexcept { return _system; }
		WINCONSTEXPR std::string getName() const noexcept { return (_name.empty() ? _sym : _name); }
		WINCONSTEXPR std::string getSymbol() const noexcept { return _sym; }

		WINCONSTEXPR bool hasName() const noexcept { return !_name.empty(); }

		CONSTEXPR bool operator==(const Unit& o) const { return _system == o._system && unitcf == o.unitcf; }

		friend std::ostream& operator<<(std::ostream& os, const Unit& u)
		{
			return os << u.getSymbol();
		}
	};

	enum class Powers : char {
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
	};

	struct System {
		using T = long double;
		using U = Unit;
	};

	/**
	 * @struct	Metric
	 * @brief	Intra-Metric-System Conversion Factors. (Relative to Meters)
	 */
	static struct : public System { // SystemID::METRIC
		const std::vector<U> units{
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::PICO)), "pm", "Picometer" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::NANO)), "nm", "Nanometer" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::MICRO)), "um", "Micrometer" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::MILLI)), "mm", "Millimeter" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::CENTI)), "cm", "Centimeter" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::DECI)), "dm", "Decimeter" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::BASE)), "m", "Meter" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::DECA)), "dam", "Decameter" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::HECTO)), "hm", "Hectometer" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::KILO)), "km", "Kilometer" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::MEGA)), "Mm", "Megameter" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::GIGA)), "Gm", "Gigameter" },
			{ SystemID::METRIC, std::pow(10.0L, static_cast<T>(Powers::TERA)), "Tm", "Terameter" }
		};

		const U* PICOMETER{ &units[0] };
		const U* NANOMETER{ &units[1] };
		const U* MICROMETER{ &units[2] };
		const U* MILLIMETER{ &units[3] };
		const U* CENTIMETER{ &units[4] };
		const U* DECIMETER{ &units[5] };
		const U* METER{ &units[6] };
		const U* DECAMETER{ &units[7] };
		const U* HECTOMETER{ &units[8] };
		const U* KILOMETER{ &units[9] };
		const U* MEGAMETER{ &units[10] };
		const U* GIGAMETER{ &units[11] };
		const U* TERAMETER{ &units[12] };

		// the base unit of the Metric system (meters)
		const U* const base{ METER };
	} Metric;

	/**
	 * @struct	Imperial
	 * @brief	Intra-Imperial-System Conversion Factors. (Relative to Feet)
	 */
	static struct : public System { // SystemID::IMPERIAL
		const std::vector<U> units{
			{ SystemID::IMPERIAL, (1.0L / 17280.0L), "Twip" },
			{ SystemID::IMPERIAL, (1.0L / 12000.0L), "th", "Thou" },
			{ SystemID::IMPERIAL, (1.0L / 36.0L), "Bc", "Barleycorn" },
			{ SystemID::IMPERIAL, (1.0L / 12.0L), "\"", "Inch" },
			{ SystemID::IMPERIAL, (1.0L / 3.0L), "h", "Hand" },
			{ SystemID::IMPERIAL, (1.0L), "\'", "Feet" },
			{ SystemID::IMPERIAL, (3.0L), "yd", "Yard" },
			{ SystemID::IMPERIAL, (66.0L), "ch", "Chain" },
			{ SystemID::IMPERIAL, (660.0L), "fur", "Furlong" },
			{ SystemID::IMPERIAL, (5280.0L), "mi", "Mile" },
			{ SystemID::IMPERIAL, (15840.0L), "lea", "League" },
			{ SystemID::IMPERIAL, (6.0761L), "ftm", "Fathom" },
			{ SystemID::IMPERIAL, (607.61L), "Cable" },
			{ SystemID::IMPERIAL, (6076.1L), "nmi", "Nautical Mile" },
			{ SystemID::IMPERIAL, (66.0L / 100.0L), "Link" },
			{ SystemID::IMPERIAL, (66.0L / 4.0L), "rd", "Rod" }
		};

		const U* TWIP{ &units[0] };
		const U* THOU{ &units[1] };
		const U* BARLEYCORN{ &units[2] };
		const U* INCH{ &units[3] };
		const U* HAND{ &units[4] };
		const U* FOOT{ &units[5] };
		const U* YARD{ &units[6] };
		const U* CHAIN{ &units[7] };
		const U* FURLONG{ &units[8] };
		const U* MILE{ &units[9] };
		const U* LEAGUE{ &units[10] };
		// maritime units
		const U* FATHOM{ &units[11] };
		const U* CABLE{ &units[12] };
		const U* NAUTICAL_MILE{ &units[13] };
		// 17th century onwards
		const U* LINK{ &units[14] };
		const U* ROD{ &units[15] };
		// the base unit of this system
		const U* const base{ FOOT };
	} Imperial;

	/// @brief	Inter-System (Metric:Imperial) Conversion Factor
	const constexpr auto ONE_FOOT_IN_METERS{ 0.3048L };
	/// @brief	Inter-System (CKUnit:Metric) Conversion Factor
	const constexpr auto ONE_UNIT_IN_METERS{ 0.0142875313L }, ONE_UNIT_IN_FEET{ 0.046875L };

	/**
	 * @brief			Converts between units in one measurement system.
	 * @param in_unit	Input Conversion Factor
	 * @param v			Input Value
	 * @param out_unit	Output Conversion Factor
	 * @returns			long double
	 */
	inline constexpr long double convert_unit(const long double& in_unitcf, const long double& v, const long double& out_unitcf)
	{
		if (math::equal(out_unitcf, 0.0L))
			throw make_exception("convert_unit() failed:  Cannot divide by zero!");
		return ((v * in_unitcf) / out_unitcf);
	}
	/**
	 * @brief				Convert between measurement systems.
	 * @param in_system		Input Measurement SystemID
	 * @param v_base		Input Value, in the input system's base unit. (Metric = Meters, Imperial = Feet)
	 * @param out_system	Output Measurement SystemID
	 * @returns				long double
	 */
	inline constexpr long double convert_system(const SystemID& in_system, const long double& v_base, const SystemID& out_system)
	{
		if (in_system == out_system) // same system
			return v_base;
		if (in_system == SystemID::METRIC && out_system == SystemID::IMPERIAL)
			return v_base / ONE_FOOT_IN_METERS;
		if (in_system == SystemID::IMPERIAL && out_system == SystemID::METRIC)
			return v_base * ONE_FOOT_IN_METERS;
		throw make_exception("convert_system() failed:  No handler exists for the given input type!");
	}

	/**
	 * @brief		Convert a number in a given unit to another unit and/or system.
	 * @param in	Input Unit.
	 * @param val	Input Value.
	 * @param out	Output Unit.
	 * @returns		long double
	 */
	inline static constexpr long double convert(const Unit& in, const long double& val, const Unit& out)
	{
		if (in.unitcf == 0.0L)
			throw make_exception("Illegal input conversion factor");
		if (out.unitcf == 0.0L)
			throw make_exception("Illegal output conversion factor");

		if (in.getSystem() == out.getSystem()) // convert between units only
			return convert_unit(in.unitcf, val, out.unitcf);
		// Convert between systems & units
		return convert_system(in.getSystem(), in.to_base(static_cast<long double>(val)), out.getSystem()) / out.unitcf;
	}

	/**
	 * @brief		Retrieve the unit specified by a string containing the unit's official symbol, or name.
	 * @param str	Input String. (This is not processed beyond case-conversion)
	 * @param def	Optional default return value if the string is invalid.
	 * @returns		Unit
	 */
	inline Unit getUnit(const std::string& str, const std::optional<Unit>& def = std::nullopt)
	{
		if (str.empty()) {
			if (def.has_value())
				return def.value();
			throw make_exception("No unit specified ; string was empty!");
		}
		const auto s{ str::tolower(str) };

		//#define DISABLE_NUTJOB_UNITS

		// BEGIN IMPERIAL //
		#ifndef DISABLE_NUTJOB_UNITS
		if (s.find("twip") < s.size())
			return *Imperial.TWIP;
		if (str == "th" || s.find("thou") < s.size())
			return *Imperial.THOU;
		if (str == "Bc" || s.find("barleycorn") < s.size())
			return *Imperial.BARLEYCORN;
		if (str == "h" || s.find("hand") < s.size())
			return *Imperial.HAND;
		if (str == "ch" || s.find("chain") < s.size())
			return *Imperial.CHAIN;
		if (str == "fur" || s.find("furlong") < s.size())
			return *Imperial.FURLONG;
		if (str == "lea" || s.find("league") < s.size())
			return *Imperial.LEAGUE;
		if (str == "ftm" || s.find("fathom") < s.size())
			return *Imperial.FATHOM;
		if (s.find("cable") < s.size())
			return *Imperial.CABLE;
		if (s.find("link") < s.size())
			return *Imperial.LINK;
		if (str == "rd" || s.find("rod") < s.size())
			return *Imperial.ROD;
		#endif // DISABLE_NUTJOB_UNITS
		if (str == "in" || s == "i" || s.find("inch") < s.size())
			return *Imperial.INCH;
		if (str == "ft" || s == "f" || s.find("foot") < s.size() || s.find("feet") < s.size())
			return *Imperial.FOOT;
		if (str == "yd" || s.find("yard") < s.size())
			return *Imperial.YARD;
		if (str == "nmi" || s.find("nauticalmile") < s.size() || s.find("nmile") < s.size())
			return *Imperial.CABLE;
		if (str == "mi" || s.find("mile") < s.size())
			return *Imperial.MILE;
		// END IMPERIAL //

		// BEGIN METRIC //
		// comparisons omit -er|-re to allow both the American and British spelling of "meter|metre".
		if (str == "pm" || s.find("picomet") < s.size())
			return *Metric.PICOMETER;
		if (str == "nm" || s.find("nanomet") < s.size())
			return *Metric.NANOMETER;
		if (str == "um" || s.find("micromet") < s.size())
			return *Metric.MICROMETER;
		if (str == "mm" || s.find("millimet") < s.size())
			return *Metric.MILLIMETER;
		if (str == "cm" || s.find("centimet") < s.size())
			return *Metric.CENTIMETER;
		if (str == "dm" || s.find("decimet") < s.size())
			return *Metric.DECIMETER;
		if (str == "dam" || s.find("decamet") < s.size())
			return *Metric.DECAMETER;
		if (str == "hm" || s.find("hectomet") < s.size())
			return *Metric.HECTOMETER;
		if (str == "km" || s.find("kilomet") < s.size())
			return *Metric.KILOMETER;
		if (str == "Mm" || s.find("megamet") < s.size())
			return *Metric.MEGAMETER;
		if (str == "Gm" || s.find("gigamet") < s.size())
			return *Metric.GIGAMETER;
		if (str == "Tm" || s.find("teramet") < s.size())
			return *Metric.TERAMETER;
		// this has to be checked after all prefix types
		if (str == "m" || s.find("met") < s.size())
			return *Metric.METER;
		// END METRIC //

		if (def.has_value())
			return def.value();

		throw make_exception("Unrecognized Unit: \"", str, '\"');
	}

	inline std::ostream& configure_ostream(std::ostream& os)
	{
		// TODO: Add stuff here
		return os;
	}

	/**
	 * @struct	Convert
	 * @brief	Performs a single conversion operation, and exposes std::ostream operator<<() to format and insert it into an output stream.
	 */
	struct Convert {
		/// @brief	String Tuple
		using Tuple = std::tuple<Unit, long double, Unit>;
		using StrTuple = std::tuple<std::string, std::string, std::string>;
		using NumberT = long double;

		Tuple _vars;
		std::streamsize _min_indent{ 0ull };

		///	@brief	Sorts the first & second arguments so that they are in the correct order when passed to the converter. Also removes any commas.
		static inline Tuple convert_tuple(std::tuple<std::string, std::string, std::string>&& tpl)
		{
			auto& [first, second, third] { tpl };

			// swap the first & second args if the first argument is the value
			if (std::all_of(first.begin(), first.end(), [](auto&& ch) { return isdigit(ch) || ch == '.' || ch == '-' || ch == ','; })) {
				const auto copy{ second };
				second = str::strip(first, ',');
				first = copy;
			}
			else second = str::strip(second, ',');

			return{ getUnit(first), str::stold(second), getUnit(third) };
		}

		///	@brief	Returns the result of the conversion.
		static inline NumberT getResult(const Unit& input_unit, const NumberT& input, const Unit& output_unit) noexcept(false)
		{
			if (math::equal(input, 0.0l)) // if input is 0, short-circuit and return 0
				return 0.0l;
			if (input_unit == output_unit)
				return input;
			return convert(input_unit, input, output_unit);
		}

		/// @brief	Default constructor
		Convert(std::tuple<std::string, std::string, std::string>&& vars, const std::streamsize& min_indent = 0ull) : _vars{ std::move(convert_tuple(std::move(vars))) }, _min_indent{ min_indent } {}
		/**
		 * @brief			Constructor
		 * @param unit_in	Input Unit (OR Input Value, if val_in is the input unit)
		 * @param val_in	Input Value (OR Input Unit, if unit_in is the input value)
		 * @param unit_out	Output Unit
		 */
		Convert(const std::string& unit_in, const std::string& val_in, const std::string& unit_out, const std::streamsize& min_indent = 0ull) : Convert(std::move(std::make_tuple(unit_in, val_in, unit_out)), min_indent) {}

		NumberT operator()() const { return getResult(std::get<0>(_vars), std::get<1>(_vars), std::get<2>(_vars)); }

		/**
		 * @brief	Format and print the result of the conversion to the given ostream instance.
		 *\n		This may change the floatfield and/or precision of the output stream.
		 * @returns	std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const Convert& conv)
		{
			// get inputs
			const auto& [input_unit, input, output_unit] {conv._vars};

			const auto
				input_unit_str{ str::stringify(configure_ostream, input_unit) },
				input_str{ str::stringify(configure_ostream, input) };

			os // insert input
				<< input_str
				<< ' '
				<< input_unit_str
				<< str::VIndent(conv._min_indent, (input_str.size() + input_unit_str.size() + 1ull))
				<< '=' << ' ';


			const NumberT output{ conv.getResult(input_unit, input, output_unit) };
			const auto output_str{ str::stringify(configure_ostream, output) };

			os << output_str << ' ' << output_unit;

			return os;
		}
	};

}
