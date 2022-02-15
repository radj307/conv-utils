/**
 * @file	PrintableEquation.hpp
 * @author	radj307
 * @brief	Objects useful when printing generic numeric equations to the console.
 */
#pragma once
#include "base.hpp"
#include "modulo.hpp"
#include "radians.hpp"
#include "unit.hpp"
#include "data.hpp"
#include "FOV.hpp"

#include <var.hpp>

class PrintableEquation final {
	using const_iterator = std::string::const_iterator;
	std::string _eq;
	bool _quiet;
	const_iterator _quiet_begin, _quiet_end;
public:
	// default ctor
	PrintableEquation() = default;

	PrintableEquation(const bool& quiet, std::string&& eq, const const_iterator& quietBegin) : _eq{ std::move(eq) }, _quiet{ quiet }, _quiet_begin{ quietBegin }, _quiet_end{ _eq.end() } {}
	PrintableEquation(const bool& quiet, std::string&& eq, const const_iterator& quietBegin, const const_iterator& quietEnd) : _eq{ std::move(eq) }, _quiet{ quiet }, _quiet_begin{ quietBegin }, _quiet_end{ quietEnd } {}

	PrintableEquation(const bool& quiet, std::string&& eq, const size_t& quietBegin) : _eq{ std::move(eq) }, _quiet{ quiet }, _quiet_begin{ _eq.begin() + quietBegin }, _quiet_end{ _eq.end() } {}
	PrintableEquation(const bool& quiet, std::string&& eq, const size_t& quietBegin, const size_t& quietEnd) : _eq{ std::move(eq) }, _quiet{ quiet }, _quiet_begin{ _eq.begin() + quietBegin }, _quiet_end{ _eq.begin() + quietEnd } {}

	// variadic ctor
	template<var::Streamable<std::stringstream>... Ts>
	PrintableEquation(const bool& quiet, Ts const&... components) : _eq{ str::stringify(components...) }, _quiet{ quiet } {}

	void setQuiet(const bool& quiet, const const_iterator& beg, const const_iterator& end)
	{
		_quiet = quiet;
		_quiet_begin = beg;
		_quiet_end = end;
	}

	auto begin() const { return _quiet ? _quiet_begin : _eq.begin(); }
	auto end() const { return _quiet ? _quiet_end : _eq.end(); }
	auto empty() const { return _eq.empty(); }

	friend std::ostream& operator<<(std::ostream& os, const PrintableEquation& eq)
	{
		for (const auto& it : eq)
			os << it;
		return os;
	}
};
