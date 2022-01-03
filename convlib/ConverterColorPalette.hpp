#pragma once
#include <palette.hpp>
#include <OutputHelper.hpp>

using PaletteT = color::palette<UIElement>;
/**
 * @brief Output Color Palette, controls the colors of various UI elements
 */
PaletteT Palette{ {
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
	}
};