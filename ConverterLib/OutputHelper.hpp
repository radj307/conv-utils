#pragma once
#include <ostream>
#include <concepts>
#include <cmath>
#include <utility>
#include <ColorPalette.hpp>
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
};

static struct { // Program-wide output settings
	bool number_grouping{ false };
	bool output_only{ false };
	bool hide_types{ false };
	std::streamsize help_margin_width{ 26ll };

	using PaletteT = color::ColorPalette<UIElement>;
	/**
	 * @brief Output Color Palette, controls the colors of various UI elements
	 */
	PaletteT Palette{ {
		{ UIElement::DATA_EQUALS, { color::intense_yellow, color::Layer::FOREGROUND, color::FormatFlag::BOLD } },
		{ UIElement::DATA_INPUT_VALUE, { color::white, color::Layer::FOREGROUND, color::FormatFlag::BOLD } },
		{ UIElement::DATA_INPUT_TYPE, color::orange },
		{ UIElement::DATA_OUTPUT_VALUE, { color::white, color::Layer::FOREGROUND, color::FormatFlag::BOLD } },
		{ UIElement::DATA_OUTPUT_TYPE, color::orange },
		{ UIElement::HEX_EQUALS, { color::intense_yellow, color::Layer::FOREGROUND, color::FormatFlag::BOLD } },
		{ UIElement::HEX_INPUT, { color::white, color::Layer::FOREGROUND, color::FormatFlag::BOLD } },
		{ UIElement::HEX_OUTPUT, { color::white, color::Layer::FOREGROUND, color::FormatFlag::BOLD } },
	} };
} OutputSettings;

/// @brief Print floating-point numbers with as-needed precision
template<class T> requires std::is_floating_point_v<T>
struct FloatPrinter {
	T _value;

	FloatPrinter(T value) : _value{ std::move(value) } {}

	operator T() const { return _value; }
	operator std::string() const
	{
		const auto str{ str::stringify(std::fixed, OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder, _value) };
		const size_t dpos{ str.find('.') }, last_pos{ str.find_last_of("123456789", dpos + 1u) };
		if (last_pos < dpos)
			return str.substr(0u, dpos);
		return str.substr(0u, dpos + last_pos);
	}

	friend std::ostream& operator<<(std::ostream& os, const FloatPrinter<T>& obj)
	{
		os << obj.operator std::string();
		return os;
	}
};
