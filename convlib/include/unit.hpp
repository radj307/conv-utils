/**
 * @file	unit.hpp
 * @author	radj307
 * @brief	Contains real-world measurement unit converters, currently supporting metric & imperial.
 */
#include <OutputHelper.hpp>

#include <sysarch.h>
#include <make_exception.hpp>
#include <str.hpp>
#include <math.hpp>

namespace unit {
	/**
	 * @enum	System
	 * @brief	Accepted Measurement Systems
	 */
	enum class System : char {
		METRIC,
		IMPERIAL,
	};

	/**
	 * @struct	Metric
	 * @brief	Intra-Metric-System Conversion Factors
	 */
	static struct {
		enum class MetricPowers : char {
			PICOMETER = -12,
			NANOMETER = -9,
			MICROMETER = -6,
			MILLIMETER = -3,
			CENTIMETER = -2,
			DECIMETER = -1,
			METER = 0,
			DECAMETER = 1,
			HECTOMETER = 2,
			KILOMETER = 3,
			MEGAMETER = 6,
			GIGAMETER = 9,
			TERAMETER = 12,
		};

		using T = long double;

		#define MAKEV_METRIC(u) const T u{ std::pow(10.0l, static_cast<long double>(MetricPowers::u)) }
		MAKEV_METRIC(PICOMETER);
		MAKEV_METRIC(NANOMETER);
		MAKEV_METRIC(MICROMETER);
		MAKEV_METRIC(MILLIMETER);
		MAKEV_METRIC(CENTIMETER);
		MAKEV_METRIC(DECIMETER);
		MAKEV_METRIC(METER);
		MAKEV_METRIC(DECAMETER);
		MAKEV_METRIC(HECTOMETER);
		MAKEV_METRIC(KILOMETER);
		MAKEV_METRIC(MEGAMETER);
		MAKEV_METRIC(GIGAMETER);
		MAKEV_METRIC(TERAMETER);
	} Metric;

	/**
	 * @struct	Imperial
	 * @brief	Intra-Imperial-System Conversion Factors
	 */
	static struct {
		using T = long double;

		const T INCH{ 1.0l / 12.0l };
		const T FOOT{ 1.0l };
		const T YARD{ 3.0l };
	} Imperial;

	/// @brief	Inter-System (Metric:Imperial) Conversion Factor
	const constexpr auto ONE_FOOT_IN_METERS{ 0.3048 };

	/**
	 * @struct	Unit
	 * @brief	POD Structure with System & conversion factor members used to represent a measurement unit.
	 */
	struct Unit {
		System system;
		long double unitcf;
		//	Unit(System sys, long double dbl) : system{ sys }, unitcf{ dbl } {}
		long double base(const long double& val) const
		{
			return val * unitcf;
		}
		CONSTEXPR bool operator==(const Unit& o) const
		{
			return system == o.system && unitcf == o.unitcf;
		}
		CONSTEXPR operator System() const { return system; }
		CONSTEXPR operator long double() const { return unitcf; }
	};

	#ifdef _DEBUG
	inline constexpr const auto is_POD{ std::is_pod_v<Unit> };
	#endif

	/**
	 * @brief			Converts between units in one measurement system.
	 * @param in_unit	Input Conversion Factor
	 * @param v			Input Value
	 * @param out_unit	Output Conversion Factor
	 * @returns			long double
	 */
	inline constexpr long double convert_unit(const long double& in_unit, const long double& v, const long double& out_unit)
	{
		return ((v * in_unit) / out_unit);
	}
	/**
	 * @brief				Convert between measurement systems.
	 * @param in_system		Input Measurement System
	 * @param v_base		Input Value, in the input system's base unit. (Metric = Meters, Imperial = Feet)
	 * @param out_system	Output Measurement System
	 * @returns				long double
	 */
	inline constexpr long double convert_system(const System& in_system, const long double& v_base, const System& out_system)
	{
		if (in_system == out_system)
			return v_base;
		if (in_system == System::METRIC && out_system == System::IMPERIAL)
			return v_base / ONE_FOOT_IN_METERS;
		if (in_system == System::IMPERIAL && out_system == System::METRIC)
			return v_base * ONE_FOOT_IN_METERS;
		throw make_exception("No handler exists for this conversion!");
	}

	/**
	 * @brief		Convert a number in a given unit to another unit and/or system.
	 * @param in	Input Unit.
	 * @param val	Input Value.
	 * @param out	Output Unit.
	 * @returns		long double
	 */
	template<var::arithmetic T>
	inline static constexpr long double convert(const Unit& in, const T& val, const Unit& out)
	{
		if (in.unitcf == 0.0l)
			throw make_exception("Illegal input conversion factor");
		if (out.unitcf == 0.0l)
			throw make_exception("Illegal output conversion factor");

		if (in.system == out.system) // convert between units only
			return convert_unit(in.unitcf, val, out.unitcf);
		// Convert between systems & units
		return convert_system(in.system, in.base(static_cast<long double>(val)), out.system) / out.unitcf;
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

		// check imperial
		// negative
		if (str == "in" || s == "i" || str::matches_any<false>(s, 4ull, "inch", "inches"))
			return Unit{ System::IMPERIAL, Imperial.INCH };
		// positive
		if (str == "yd" || s == "y" || str::matches_any<false>(s, 4ull, "yard", "yards"))
			return Unit{ System::IMPERIAL, Imperial.YARD };
		// base
		if (str == "ft" || s == "f" || str::matches_any<false>(s, 4ull, "foot", "feet"))
			return Unit{ System::IMPERIAL, Imperial.FOOT };

		// check metric (str == (symbol) || str == (name))
		// negative
		if (str == "pm" || str::matches_any<false>(s, 7ull, "picometer"))
			return Unit{ System::METRIC, Metric.PICOMETER };
		if (str == "nm" || str::matches_any<false>(s, 7ull, "nanometer"))
			return Unit{ System::METRIC, Metric.NANOMETER };
		if (str == "µm" || str == "um" || str::matches_any<false>(s, 8ull, "micrometer"))
			return Unit{ System::METRIC, Metric.MICROMETER };
		if (str == "mm" || str::matches_any<false>(s, 8ull, "millimeter"))
			return Unit{ System::METRIC, Metric.MILLIMETER };
		if (str == "cm" || str::matches_any<false>(s, 8ull, "centimeter"))
			return Unit{ System::METRIC, Metric.CENTIMETER };
		if (str == "dm" || str::matches_any<false>(s, 7ull, "decimeter"))
			return Unit{ System::METRIC, Metric.DECIMETER };
		// positive
		if (str == "dam" || str::matches_any<false>(s, 7ull, "decameter"))
			return Unit{ System::METRIC, Metric.DECAMETER };
		if (str == "hm" || str::matches_any<false>(s, 8ull, "hectometer"))
			return Unit{ System::METRIC, Metric.HECTOMETER };
		if (str == "km" || str::matches_any<false>(s, 7ull, "kilometer"))
			return Unit{ System::METRIC, Metric.KILOMETER };
		if (str == "Mm" || str::matches_any<false>(s, 7ull, "megameter"))
			return Unit{ System::METRIC, Metric.MEGAMETER };
		if (str == "Gm" || str::matches_any<false>(s, 7ull, "gigameter"))
			return Unit{ System::METRIC, Metric.GIGAMETER };
		if (str == "Tm" || str::matches_any<false>(s, 7ull, "terameter"))
			return Unit{ System::METRIC, Metric.TERAMETER };
		// base
		if (str == "m" || str::matches_any<false>(s, 3ull, "meter"))
			return Unit{ System::METRIC, Metric.METER };

		if (def.has_value())
			return def.value();
		throw make_exception("Invalid unit: \"", str, '\"');
	}

	struct Conversion {
		std::string in_unit, in_val, out_unit;
		Conversion(const std::string& in_unit, const std::string& value, const std::string& out_unit) : in_unit{ in_unit }, in_val{ value }, out_unit{ out_unit } {}
		Conversion(const std::tuple<std::string, std::string, std::string>& vars) : in_unit{ std::get<0>(vars) }, in_val{ std::get<1>(vars) }, out_unit{ std::get<2>(vars) } {}
		friend std::ostream& operator<<(std::ostream& os, const Conversion& conv)
		{
			if (std::all_of(conv.in_val.begin(), conv.in_val.end(), [](auto&& ch) { return isdigit(ch) || ch == '.' || ch == '-'; })) {
				const auto in_val{ str::stold(conv.in_val) }, out_val{ convert(getUnit(conv.in_unit), in_val, getUnit(conv.out_unit)) };
				std::string out_str{ str::strip_trailing(str::stringify(std::fixed, std::setprecision(OutputSettings.precision), out_val), '0') };
				if (out_str.empty())
					throw make_exception("Precision isn't set high enough for this operation!");
				if (out_str.back() == '.')
					out_str.pop_back();
				os << (OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder) << Palette.set(UIElement::UNIT_VALUE) << conv.in_val << Palette.reset() << ' ' << Palette.set(UIElement::UNIT_INPUT) << conv.in_unit << Palette.reset() << " = " << Palette.set(UIElement::UNIT_VALUE) << out_str << Palette.reset() << ' ' << Palette.set(UIElement::UNIT_OUTPUT) << conv.out_unit << Palette.reset();

				#ifdef _DEBUG
				const auto p{ os.precision(32) };
				os << '\n' << term::debug << "Out Val: (" << out_val << ")";
				os.precision(p);
				#endif
			}
			else throw make_exception("Invalid Number: \"", conv.in_val, "\"!");
			return os;
		}
	};
}