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
		const auto& is_value_arg{ [](const std::string_view& str) -> bool {
			return std::all_of(str.begin(), str.end(), [](auto&& ch) { return isdigit(ch) || ch == '.' || ch == '-'; });
		} };
		// lambda that retrieves 3 arguments and puts them into a tuple. DOES NOT CHECK BOUNDS!
		const auto& get_conv_args{ [&is_value_arg](auto&& it) {
			const auto first{ *it };
			const auto second{ *++it };
			const auto third{ *++it };
			if (is_value_arg(second))
				return std::make_tuple(first, second, third); // <in-unit> <val> <out-unit>
			else return std::make_tuple(second, first, third);// <val> <in-unit> <out-unit>
		} };

		(std::cout << std::fixed).precision(OutputSettings.precision);
		for (auto it{ params.begin() }; it < params.end(); ++it)
			if (std::distance(it, params.end()) >= 2) ///< check bounds before calling get_conv_args
				std::cout << unit::Convert(get_conv_args(it)) << std::endl;
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
						throw make_exception("Too many '%' characters in parameter \"", *it, "\"");
					here = here.substr(0ull, pos);
				}
				else if (it != params.end() - 1ull)
					next = *++it;
				else
					throw make_exception("Unmatched value: \"", here, "\"");
				modulo::operation(here, next);
			} catch (std::exception& ex) {
				std::cerr << term::error << ex.what() << std::endl;
			}
		}
	}
}