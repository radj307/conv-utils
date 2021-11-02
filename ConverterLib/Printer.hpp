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