#pragma once
#include <ostream>
#include <concepts>
#include <cmath>
#include <utility>
#include <str.hpp>

/**
 * @enum UIElement
 * @brief Various Output UI Elements, used by the color palette to easily select output colors.
 */
enum class UIElement {
	DATA_EQUALS,
	DATA_INPUT_VALUE,
	DATA_INPUT_TYPE,
	DATA_OUTPUT_VALUE,
	DATA_OUTPUT_TYPE,
	HEX_EQUALS,
	HEX_INPUT,
	HEX_OUTPUT,
	MOD_EQUALS,
	MOD_MODULO,
	MOD_INPUT,
	MOD_OUTPUT,
	UNIT_VALUE,		// unit conversions -- input/output values
	UNIT_INPUT,		// unit conversions -- input unit
	UNIT_OUTPUT,	// unit conversions -- output unit
};

/**
 * @brief Output Color Palette, controls the colors of various UI elements
 */
color::palette<UIElement> Palette{ {
	{ UIElement::DATA_EQUALS, { color::intense_yellow, color::format::BOLD } },
	{ UIElement::DATA_INPUT_VALUE, { color::white, color::format::BOLD } },
	{ UIElement::DATA_INPUT_TYPE, color::orange },
	{ UIElement::DATA_OUTPUT_VALUE, { color::white, color::format::BOLD } },
	{ UIElement::DATA_OUTPUT_TYPE, color::orange },
	{ UIElement::HEX_EQUALS, { color::intense_yellow, color::format::BOLD } },
	{ UIElement::HEX_INPUT, { color::white, color::format::BOLD } },
	{ UIElement::HEX_OUTPUT, { color::white, color::format::BOLD } },
	{ UIElement::MOD_EQUALS, { color::intense_yellow, color::format::BOLD } },
	{ UIElement::MOD_MODULO, { color::intense_yellow, color::format::BOLD } },
	{ UIElement::MOD_INPUT, { color::white, color::format::BOLD } },
	{ UIElement::MOD_OUTPUT, { color::white, color::format::BOLD } },
	{ UIElement::UNIT_VALUE, { color::white, color::format::BOLD } },
	{ UIElement::UNIT_INPUT, { color::orange } },
	{ UIElement::UNIT_OUTPUT, { color::orange } },
	}
};

static struct { // Program-wide output settings
	bool number_grouping{ false };
	bool output_only{ false };
	bool hide_types{ false };
	size_t precision{ 8ull };
} OutputSettings;

/// @brief Print floating-point numbers with as-needed precision
template<typename T> requires std::is_floating_point_v<T>
struct FloatPrinter {
	const T _value;

	/// @brief Default Constructor. @param value - A templated floating-point value.
	constexpr FloatPrinter(T value) : _value{ std::move(value) } {}

	constexpr operator T() const { return _value; }
	constexpr operator const std::string() const
	{
		const auto str{ str::stringify(std::fixed, std::setprecision(OutputSettings.precision), OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder, _value) };
		const size_t dpos{ str.find('.') }, last_pos{ str.substr(dpos + 1ull).find_last_of("123456789") };
		if (!str::pos_valid(last_pos))
			return str.substr(0ull, dpos);
		if (last_pos < dpos)
			return str.substr(0ull, dpos);
		return str.substr(0ull, dpos + last_pos);
	}

	friend std::ostream& operator<<(std::ostream& os, const FloatPrinter<T>& obj)
	{
		os << obj.operator const std::string();
		return os;
	}
};

template<typename T> requires std::is_integral_v<T>
struct IntPrinter {
	const T _value;

	constexpr IntPrinter(T value) : _value{ std::move(value) } {}

	constexpr operator T() const { return _value; }
	constexpr operator const std::string() const
	{
		return str::stringify(std::fixed, OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder, _value);
	}

	friend std::ostream& operator<<(std::ostream& os, const IntPrinter<T>& obj)
	{
		os << obj.operator const std::string();
		return os;
	}
};
