#pragma once
#include <str.hpp>
#include <OutputHelper.hpp>
#include <data.hpp>
#include <base.hpp>
#include <modulo.hpp>
#include <Printer.hpp>
#include "CompileHelpDoc.hpp"

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
		for (auto& arg : params)
			std::cout << base::negative_abstractor(arg) << '\n';
	}

	// MODE: Modulo Calculator
	inline void mode_mod(const std::vector<opt::Parameter>& params)
	{
		for (auto it{ params.begin() }; it != params.end(); ++it) {
			try {
				std::string here{ *it }, next{ "" };
				if (const auto pos{ here.find('%') }; str::pos_valid(pos)) {
					next = here.substr(pos + 1ull);
					if (str::pos_valid(next.find('%')))
						throw std::exception(std::string("Too many '%' characters in parameter \"" + *it + "\"").c_str());
					here = here.substr(0ull, pos);
				}
				else if (it != params.end() - 1ull)
					next = *++it;
				else
					throw std::exception(std::string("Unmatched value: \"" + here + "\"").c_str());
				modulo::operation(here, next);
			} catch (std::exception& ex) {
				std::cout << sys::term::error << ex.what() << std::endl;
			}
		}
	}
}