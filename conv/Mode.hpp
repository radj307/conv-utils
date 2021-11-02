#pragma once
#include <str.hpp>
#include <OutputHelper.hpp>
#include <data.hpp>
#include <base.hpp>
#include <Printer.hpp>
/**
 * @namespace conv_mode
 * @brief Contains functions that control the different modes available in the conv program.
 */
namespace conv_mode {
	// MODE: Help
	inline void mode_help(const std::string& usage_str, const std::vector<ArgumentPair>& modes, const std::vector<ArgumentPair>& options)
	{
		for (auto& ln : compile_help_doc(usage_str, modes, options))
			std::cout << ln << '\n';
	}

	// MODE: Data Size Conversions
	inline void mode_data(const std::vector<opt::Parameter>& params)
	{
		for (auto arg{ params.begin() }; arg != params.end(); ++arg)
			std::cout << data::Conversion(arg, params.end()) << '\n';
	}

	// MODE: Hexadecimal Conversions
	inline void mode_hex(const std::vector<opt::Parameter>& params)
	{
		for (auto arg{ params.begin() }; arg != params.end(); ++arg) {
			bool negative{ arg->front() == '-' };
			if (std::string argstr{ negative ? arg->substr(1u) : *arg }; argstr.find("0x") == 0u || std::any_of(argstr.begin(), argstr.end(), [](auto&& c) { const auto ch{ str::toupper(c) };  return ch >= 'A' && ch <= 'F'; })) // hex input
				std::cout << IntPrinter(negative ? -base::to_decimal(argstr) : base::to_decimal(argstr)) << '\n';
			else // decimal input:
				std::cout << (negative ? "-" : "") << (OutputSettings.hide_types ? "" : "0x") << str::hex(str::stoll(argstr), std::uppercase, (OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder)) << '\n';
		}
	}
}