#pragma once
#include <TermAPI.hpp>
#include <palette.hpp>

namespace conv2 {
	enum class OUTCOLOR : unsigned char {
		NONE,
		INPUT,
		OUTPUT,
		OPERATOR,
		HIGHLIGHT,
	};
	term::palette<OUTCOLOR> color{
		std::make_pair(OUTCOLOR::NONE, color::white),
		std::make_pair(OUTCOLOR::INPUT, color::yellow),
		std::make_pair(OUTCOLOR::OUTPUT, color::green),
		std::make_pair(OUTCOLOR::OPERATOR, color::white),
		std::make_pair(OUTCOLOR::HIGHLIGHT, color::red),
	};
}
