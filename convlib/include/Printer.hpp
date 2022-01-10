#pragma once
#include <data.hpp>
#include <ConverterColorPalette.hpp>

namespace data { // DATA OUTPUT OPERATORS
	// stream output operator for conversion
	inline std::ostream& operator<<(std::ostream& os, const Conversion& printer)
	{
		if (printer._in.has_value() && printer._out.has_value()) {
			if (!OutputSettings.output_only) { // print input values
				const auto in{ printer._in.value().get() };
				os << Palette.set(UIElement::DATA_INPUT_VALUE) << FloatPrinter(in->_value) << Palette.reset();
				if (!OutputSettings.hide_types) {
					os << ' '
						<< Palette.set(UIElement::DATA_INPUT_TYPE) << in->_type << Palette.reset();
				}
				os << Palette.set(UIElement::DATA_EQUALS) << " = " << Palette.reset();
			}
			const auto out{ printer._out.value().get() };
			os << Palette.set(UIElement::DATA_OUTPUT_VALUE) << FloatPrinter(out->_value) << Palette.reset();
			if (!OutputSettings.hide_types) {
				os << ' '
					<< Palette.set(UIElement::DATA_OUTPUT_TYPE) << out->_type << Palette.reset();
			}
		}
		return os;
	}
}
namespace base {
	inline std::string get_conversion_prefix(const std::string& in)
	{
		return (OutputSettings.output_only ? std::string{} :
										str::stringify(
											Palette.set(UIElement::HEX_INPUT),
											in,
											Palette.reset(),
											Palette.set(UIElement::HEX_EQUALS),
											" = ",
											Palette.reset(),
											Palette.set(UIElement::HEX_OUTPUT)));
	}

	inline std::string hexconv(std::string arg)
	{
		switch (detect_base(arg)) {
		case ValueBase::DECIMAL:
			return str::stringify(get_conversion_prefix(arg), str::stringify(std::hex, str::stoll(arg), std::uppercase, (OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder), (OutputSettings.hide_types ? "" : "0x")), Palette.reset());
		case ValueBase::HEXADECIMAL:
			return str::stringify(get_conversion_prefix(arg), IntPrinter(to_decimal(arg)), Palette.reset());
		case ValueBase::INVALID: [[fallthrough]];
		default:
			throw std::exception(str::stringify("Invalid number: \"", arg, "\"!").c_str());
		}
	}
}
namespace modulo {
	inline std::string get_output_prefix(const std::string& input, const std::string& mod_v)
	{
		return OutputSettings.output_only ? std::string{} : str::stringify(
			Palette.set(UIElement::MOD_INPUT),
			input,
			Palette.reset(),
			' ',
			Palette.set(UIElement::MOD_MODULO),
			'%',
			Palette.reset(),
			' ',
			Palette.set(UIElement::MOD_INPUT),
			mod_v,
			Palette.reset(),
			' ',
			Palette.set(UIElement::MOD_EQUALS),
			'=',
			Palette.reset(),
			' ');
	}

	/**
	 * @brief Stream insertion operator for the Calculate object.
	 * @param os	- Target Output Stream
	 * @param eq	- Constant Calculate Object
	 * @returns std::ostream&
	 */
	template<numtype T> inline std::ostream& operator<<(std::ostream& os, const Calculate<T>& eq)
	{
		if (!OutputSettings.output_only) {
			if constexpr (std::same_as<T, FloatT>)
				os << get_output_prefix(str::to_string(eq.in, OutputSettings.precision), str::to_string(eq.mod, OutputSettings.precision));
			else if constexpr (std::same_as<T, IntT>)
				os << get_output_prefix(std::to_string(eq.in), std::to_string(eq.mod));
		}
		os << Palette.set(UIElement::MOD_OUTPUT) << eq.getResult() << Palette.reset();
		return os;
	}

	/**
	 * @brief Selects a number type for the given parameters, then prints out the resulting equation.
	 * @param number	- Number to modulo with the second parameter.
	 * @param mod_v		- Modulo value.
	 */
	inline void operation(const std::string& number, const std::string& mod_v)
	{
		switch (find_num_type(number, mod_v)) {
		case NumberType::FLOAT: // if at least one input is a floating-point, cast both to long double & get result
			std::cout << modulo::Calculate<FloatT>(str::stold(number), str::stold(mod_v)) << '\n';
			break;
		case NumberType::INT: // if neither input is a floating-point, use integers.
			std::cout << modulo::Calculate<IntT>(str::stoll(number), str::stoll(mod_v)) << '\n';
			break;
		}
	}
}