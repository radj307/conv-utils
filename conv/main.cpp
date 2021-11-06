#include <TermAPI.hpp>
#include <ParamsAPI.hpp>
#include <resolve-path.hpp>
#include <str.hpp>

#include "Mode.hpp"
using namespace conv_mode;

/// @brief Check for setting arguments and apply to settings
inline void init_settings_from_args(opt::ParamsAPI& args);

// MAIN
int main(const int argc, char** argv, char** envp)
{
	try {
		std::cout << sys::term::EnableANSI << std::fixed; // enable ANSI, force standard notation
		opt::ParamsAPI args{ argc, argv };
		const auto [program_path, program_name] { opt::resolve_split_path(envp, args.arg0().value_or(argv[0])) };

		init_settings_from_args(args); // init settings

		const bool do_help{ args.empty() || check_args(args, MODE_HELP) };

		// Check if args are empty / help arg included
		if (do_help)
			mode_help(program_name + " <MODE> [OPTIONS] [PARAMETERS]",
					  { MODE_HELP, MODE_DATA, MODE_HEX, MODE_MOD },
					  { NUMBER_GROUPING, PRECISION, OUTPUT_ONLY, HIDE_TYPES, NO_COLOR }
		);
		// Get all parameters
		const auto params{ args.getAllParameters<std::vector<opt::Parameter>>() };

		// Select primary mode
		if (check_args(args, MODE_DATA)) // convert params as data sizes
			mode_data(params);
		else if (check_args(args, MODE_HEX)) // convert params as hexadecimal/decimal
			mode_hex(params);
		else if (check_args(args, MODE_MOD))
			mode_mod(params);
		else if (!do_help)
			throw std::exception("No mode was specified!");

		return 0;
	} catch (std::exception& ex) { // catch std::exception
		std::cout << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch (...) { // catch unknown exception
		std::cout << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}

inline void init_settings_from_args(opt::ParamsAPI& args)
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
		Palette._active = false;

	if (check_args(args, PRECISION)) {
		if (const auto prec{ args.getv(PRECISION._arg.opt()) }; prec.has_value())
			OutputSettings.precision = str::stoull(prec.value());
		else
			throw std::exception(str::stringify("Precision argument did not specify an unsigned integral!").c_str());
	}
}