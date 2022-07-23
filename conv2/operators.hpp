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
namespace exponents {
	INLINE std::pair<std::string, std::string> getOperationResult(std::string const& rawInput, const bool quiet)
	{
		using conv2::OUTCOLOR;
		using conv2::color;

		static const std::regex rgxParseOperation{ "(?:\\({0,1}?\\s*([\\s\\d\\^]+)\\s*\\){0,1})\\s*\\^\\s*(?:\\({0,1}\\s*([\\s\\d\\^]+)\\s*\\){0,1})", std::regex_constants::optimize };
		std::smatch matches;

		if (!std::regex_search(rawInput, matches, rgxParseOperation))
			throw make_exception("Unrecognized operation syntax '", rawInput, "'");

		if (!matches[1].matched && !matches[2].matched)
			throw make_exception("Missing operand and exponent in '", rawInput, "'");
		else if (!matches[1].matched)
			throw make_exception("Missing operand in '", rawInput, "'");
		else if (!matches[2].matched)
			throw make_exception("Missing exponent in '", rawInput, "'");

		std::string num{ str::trim(matches[1].str()) }, exp{ str::trim(matches[2].str()) };
		std::string numEq, expEq;

		// resolve sub-expressions, if necessary
		if (num.find('^') != std::string::npos) {
			const auto& [eq, result] {getOperationResult(num, quiet) };
			numEq = eq;
			num = result;
		}
		if (exp.find('^') != std::string::npos) {
			const auto& [eq, result] { getOperationResult(exp, quiet) };
			expEq = eq;
			exp = result;
		}

		std::string result{ Pow(num, exp).getResultString() };
		std::stringstream ss;

		if (!quiet) {
			// NUMBER:
			if (numEq.empty())
				ss << color(OUTCOLOR::INPUT) << num << color();
			else {
				ss << color(OUTCOLOR::HIGHLIGHT) << '(' << color();
				ss << color(OUTCOLOR::INPUT) << numEq << color();
				ss << color(OUTCOLOR::HIGHLIGHT) << ')' << color();
			}
			// CARET:
			ss << ' ' << color(OUTCOLOR::OPERATOR) << '^' << color() << ' ';
			// EXPONENT:
			if (expEq.empty())
				ss << color(OUTCOLOR::INPUT) << exp << color();
			else {
				ss << color(OUTCOLOR::HIGHLIGHT) << '(' << color();
				ss << color(OUTCOLOR::INPUT) << expEq << color();
				ss << color(OUTCOLOR::HIGHLIGHT) << ')' << color();
			}
			// EQUALS:
			ss << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
		}
		ss << color(OUTCOLOR::OUTPUT) << result << color();
		return { ss.str(), result };
	}
}
