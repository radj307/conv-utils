#include <TermAPI.hpp>
#include <ParamsAPI.hpp>
#include <resolve-path.hpp>
#include <binary.hpp>
#include <ColorPalette.hpp>
#include <Argument.hpp>
#include <hex.hpp>

#include <str.hpp>

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
		if (arg->find("0x") == 0u || std::any_of(arg->begin(), arg->end(), [](auto&& c) { const auto ch{ str::toupper(c) };  return ch >= 'A' && ch <= 'F'; }) || arg->front() == '0') // hex input
			std::cout << IntPrinter(hex::to_decimal(*arg, 16)) << '\n';
		else // decimal input
			std::cout << (OutputSettings.hide_types ? "" : "0x") << str::hex(str::stoll(*arg), std::uppercase, (OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder)) << '\n';
	}
}

/// @brief Check for setting arguments and apply to settings
inline void init_settings_from_args(opt::ParamsAPI& args);

#include <TermAPIQuery.hpp>
// MAIN
int main(const int argc, char** argv, char** envp)
{
	try {
		std::cout << sys::term::EnableANSI << std::fixed; // enable ANSI, force standard notation
		opt::ParamsAPI args{ argc, argv };
		const auto [program_path, program_name]{ opt::resolve_split_path(envp, args.arg0().value_or(argv[0])) };

		init_settings_from_args(args); // init settings

		const bool do_help{ args.empty() || check_args(args, HELP) };

		// Check if args are empty / help arg included
		if (do_help)
			mode_help(program_name + " <MODE> [OPTIONS] [PARAMETERS]", { HELP, CONVERT_DATASIZE, CONVERT_HEXADECIMAL }, { NUMBER_GROUPING, OUTPUT_ONLY, HIDE_TYPES, NO_COLOR });

		// Get all parameters
		const auto params{ args.getAllParameters<std::vector<opt::Parameter>>() };

		// Select primary mode
		if (check_args(args, CONVERT_DATASIZE)) // convert params as data sizes
			mode_data(params);
		else if (check_args(args, CONVERT_HEXADECIMAL)) // convert params as hexadecimal/decimal
			mode_hex(params);
		else if (!do_help)
			throw std::exception("No mode was specified!");
		
		return 0;
	} catch ( std::exception& ex ) { // catch std::exception
		std::cout << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch ( ... ) { // catch unknown exception
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
		OutputSettings.Palette._active = false;
}