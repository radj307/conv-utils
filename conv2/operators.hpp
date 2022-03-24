#pragma once
#include "globals.h"

#include <tokenizers/bitwise.hpp>

namespace bitwise2 {
	inline std::ostream& operator<<(std::ostream& os, const operation& op)
	{
		const bool is_negate{ op.type == Operator::NEGATE };
		if (is_negate)
			os << '~';
		if (auto* subop = std::get_if<operation::suboperation>(&op.left)) {
			auto& subop_ref{ *(subop->get()) };
			const auto& enclose{ subop_ref.type != Operator::NEGATE };
			os << (enclose ? "(" : "") << subop_ref << (enclose ? ")" : ""); // recurse
		}
		else if (auto* value = std::get_if<operand>(&op.left))
			os << *value;
		if (!is_negate) {
			os << ' ' << op.type << ' ';
			if (auto* subop = std::get_if<operation::suboperation>(&op.right)) {
				auto& subop_ref{ *(subop->get()) };
				const auto& enclose{ subop_ref.type != Operator::NEGATE };
				os << (enclose ? "(" : "") << subop_ref << (enclose ? ")" : ""); // recurse
			}
			else if (auto* value = std::get_if<operand>(&op.right))
				os << *value;
		}
		return os;
	}
}
