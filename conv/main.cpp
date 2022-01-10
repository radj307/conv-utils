#include <make_exception.hpp>
#include <TermAPI.hpp>
#include <ParamsAPI2.hpp>
#include <env.hpp>
#include <str.hpp>

#include "Mode.hpp"
using namespace conv;

/// @brief Check for setting arguments and apply to settings
inline void init_settings_from_args(opt::ParamsAPI2& args);

// MAIN
int main(const int argc, char** argv)
{
	try {
		std::cout << term::EnableANSI << std::fixed; // enable ANSI, force standard notation
		opt::ParamsAPI2 args{ argc, argv, "precision" };
		env::PATH path;
		const auto [program_path, program_name] { path.resolve_split(args.arg0().value_or(argv[0])) };

		std::vector<opt::ArgContainerType> operations;
		const auto mode_tuple{ std::make_tuple(MODE_DATA.flag(), MODE_DATA.opt(), MODE_HEX.flag(), MODE_HEX.opt(), MODE_MOD.flag(), MODE_MOD.opt(), MODE_UNIT.flag(), MODE_UNIT.opt()) };
		for (auto pos{ args.find_any<opt::Flag, opt::Option>(mode_tuple, args.begin(), args.end()) }, last{ pos }; pos != args.end(); last = pos, pos = args.find_any<opt::Flag, opt::Option>(mode_tuple, pos + 1, args.end())) {
			operations.emplace_back(args.get_range(last, pos));
		}

		init_settings_from_args(args); // init settings

		const bool do_help{ args.empty() || check_args(args, MODE_HELP) };

		// Check if args are empty / help arg included
		if (do_help)
			mode::help(program_name.generic_string() + " <MODE> [OPTIONS] [PARAMETERS]",
					  { MODE_HELP, MODE_DATA, MODE_HEX, MODE_MOD, MODE_UNIT },
					  { NUMBER_GROUPING, PRECISION, OUTPUT_ONLY, HIDE_TYPES, NO_COLOR }
		);
		// Get all parameters
		const auto params{ args.typegetv_all<opt::Parameter>() };

		// Select primary mode
		if (check_args(args, MODE_DATA)) // convert params as data sizes
			mode::data_conv(params);
		else if (check_args(args, MODE_HEX)) // convert params as hexadecimal/decimal
			mode::hex_conv(params);
		else if (check_args(args, MODE_UNIT))
			mode::unit_conv(params);
		else if (check_args(args, MODE_MOD)) // calculate modulo operations
			mode::modulo_calc(params);
		else if (!do_help)
			throw std::exception("No mode was specified!");

		return 0;
	} catch (const std::exception& ex) { // catch std::exception
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) { // catch unknown exception
		std::cerr << term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}

inline void init_settings_from_args(opt::ParamsAPI2& args)
{
	// check number grouping args
	if (check_args(args, NUMBER_GROUPING))
		OutputSettings.number_grouping = true;

	// check output only args
	if (check_args(args, OUTPUT_ONLY))
		OutputSettings.output_only = true;

	// check for the hide types argument
	if (check_args(args, HIDE_TYPES))
		OutputSettings.hide_types = true;

	// check for the no color argument
	if (check_args(args, NO_COLOR))
		Palette.setActive(false);

	if (const auto prec{ args.typegetv<opt::Option>(PRECISION.opt()) }; prec.has_value())
		OutputSettings.precision = str::stoull(prec.value());
	
}