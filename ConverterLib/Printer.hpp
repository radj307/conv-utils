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
				os << Palette.set(UIElement::DATA_INPUT_VALUE) << FloatPrinter(in->_value) << color::reset;
				if (!OutputSettings.hide_types) {
					os << ' '
						<< Palette.set(UIElement::DATA_INPUT_TYPE) << in->_type << color::reset;
				}
				os << Palette.set(UIElement::DATA_EQUALS) << " = " << color::reset;
			}
			const auto out{ printer._out.value().get() };
			os << Palette.set(UIElement::DATA_OUTPUT_VALUE) << FloatPrinter(out->_value) << color::reset;
			if (!OutputSettings.hide_types) {
				os << ' '
					<< Palette.set(UIElement::DATA_OUTPUT_TYPE) << out->_type << color::reset;
			}
		}
		return os;
	}
}
namespace base {
	inline std::string get_conversion_prefix(const std::string& in, const bool negative)
	{
		return (negative ? "-" : "") + (OutputSettings.output_only ? std::string{} :
										str::stringify(
										Palette.set(UIElement::HEX_INPUT),
										in,
										color::reset,
										Palette.set(UIElement::HEX_EQUALS),
										" = ",
										color::reset,
										Palette.set(UIElement::HEX_OUTPUT)));
	}

	inline std::string negative_abstractor(std::string arg)
	{
		arg.erase(std::remove_if(arg.begin(), arg.end(), isspace), arg.end()); // remove any whitespace from illegal input
		bool is_negative{ arg.front() == '-' };
		if (is_negative)
			arg = arg.substr(1u); // strip negative
		switch (detect_base(arg)) {
		case ValueBase::DECIMAL:
			return str::stringify(get_conversion_prefix(arg, is_negative), str::hex(str::stoll(arg), std::uppercase, (OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder), (is_negative ? "-0x" : (OutputSettings.hide_types ? "" : "0x"))), color::reset);
		case ValueBase::HEXADECIMAL:
			return str::stringify(get_conversion_prefix(arg, is_negative), IntPrinter(is_negative ? -to_decimal(arg) : to_decimal(arg)), color::reset);
		case ValueBase::INVALID: [[fallthrough]];
		default:
			throw std::exception(str::stringify("Invalid number: \"", (is_negative ? "-" : ""), arg, "\"!").c_str());
		}
	}
}
namespace modulo {
	inline std::string get_output_prefix(const std::string& input, const std::string& mod_v)
	{
		return OutputSettings.output_only ? std::string{} : str::stringify(
			Palette.set(UIElement::MOD_INPUT),
			input,
			color::reset,
			' ',
			Palette.set(UIElement::MOD_MODULO),
			'%',
			color::reset,
			' ',
			Palette.set(UIElement::MOD_INPUT),
			mod_v,
			color::reset,
			' ',
			Palette.set(UIElement::MOD_EQUALS),
			'=',
			color::reset,
			' ',
			Palette.set(UIElement::MOD_OUTPUT));
	}

	inline std::ostream& operator<<(std::ostream& os, const Conversion& conv)
	{
		os << get_output_prefix(conv._in_str, conv._mod_str) << conv.get_output_str() << color::reset;
		return os;
	}
}