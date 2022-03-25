/**
 * @file	operators.hpp
 * @author	radj307
 * @brief	Contains output stream operators for recursive operations that require them.
 */
#pragma once
#include "globals.h"

#include <bitwise.hpp>

namespace bitwise {
	inline std::ostream& operator<<(std::ostream& os, const operation& op)
	{
		using conv2::OUTCOLOR;
		using conv2::color;

		const auto& printSubOperation{ [&os](operation& subop_ref) {
			const auto& enclose{ subop_ref.type != Operator::NEGATE };
			// recurse: (don't apply colors to subop_ref)
			os << color(OUTCOLOR::OPERATOR) << (enclose ? "(" : "") << color() << subop_ref << color(OUTCOLOR::OPERATOR) << (enclose ? ")" : "") << color();
		} };

		const bool is_negate_type{ op.type == Operator::NEGATE };
		if (is_negate_type)
			os << color(OUTCOLOR::OPERATOR) << '~' << color();
		if (auto* subop = std::get_if<operation::suboperation>(&op.left))
			printSubOperation(*subop->get());
		else if (auto* value = std::get_if<operand>(&op.left))
			os << color(OUTCOLOR::INPUT) << *value << color();
		if (!is_negate_type) {
			os << ' ' << color(OUTCOLOR::OPERATOR) << op.type << color() << ' ';
			if (auto* subop = std::get_if<operation::suboperation>(&op.right))
				printSubOperation(*subop->get());
			else if (auto* value = std::get_if<operand>(&op.right))
				os << color(OUTCOLOR::INPUT) << *value << color();
		}
		return os;
	}
}
