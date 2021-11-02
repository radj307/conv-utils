#pragma once
#include <ColorPalette.hpp>
#include <OutputHelper.hpp>

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
	}
};