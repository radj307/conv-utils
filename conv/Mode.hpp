#pragma once
#include <str.hpp>
#include <OutputHelper.hpp>
#include <data.hpp>
#include <base.hpp>
#include <modulo.hpp>
#include <unit.hpp>
#include <Printer.hpp>
#include "CompileHelpDoc.hpp"

/**
 * @namespace conv_mode
 * @brief Contains functions that control the different modes available in the conv program.
 */
namespace conv::mode {
	// MODE: Help
	inline void help(const std::string& usage_str, const std::vector<ArgumentPair>& modes, const std::vector<ArgumentPair>& options)
	{
		for (auto& ln : compile_help_doc(usage_str, modes, options))
			std::cout << ln << '\n';
	}

	// MODE: Data Size Conversions
	inline void data_conv(const std::vector<std::string>& params)
	{
		for (auto arg{ params.begin() }; arg != params.end(); ++arg)
			std::cout << data::Conversion(arg, params.end()) << '\n';
	}

	// MODE: Hexadecimal Conversions
	inline void hex_conv(const std::vector<std::string>& params)
	{
		for (auto& arg : params)
			std::cout << base::hexconv(arg) << '\n';
	}

	// MODE: Length Measurement Unit Conversion
	inline void unit_conv(const std::vector<std::string>& params)
	{
		(std::cout << std::fixed).precision(10);
		for (auto it{ params.begin() }; it < params.end(); ++it) {
			if (std::distance(it, params.end()) >= 2) {
				const auto in_u{ *it };
				const auto in_v{ *++it };
				const auto out_u{ *++it };
				std::cout << unit::Conversion(std::make_tuple(in_u, in_v, out_u)) << std::endl;
			}
		}
	}

	// MODE: Modulo Calculator
	inline void modulo_calc(const std::vector<std::string>& params)
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
				std::cerr << term::error << ex.what() << std::endl;
			}
		}
	}
}