#include <TermAPI.hpp>
#include <ParamsAPI.hpp>
#include <resolve-path.hpp>
#include <binary.hpp>
#include <ColorPalette.hpp>
#include <Argument.hpp>
#include <hex.hpp>

#include <str.hpp>

inline std::vector<std::string> compile_help_doc(const std::string& usage_str, std::vector<ArgumentPair> modes, std::vector<ArgumentPair> options)
{
	std::vector<std::string> lines;

	const auto compile_argument_vector{ [&lines](const std::vector<ArgumentPair>& vec) {
		for (auto& [arg, desc] : vec) {
			const auto ln{ str::stringify(str::stringify(arg.has_flag() ? std::string("-") + arg.flag() : "  ", "  ", arg.has_opt() ? std::string("--") + arg.opt() : "")) };
			lines.emplace_back(str::stringify("  ", ln, str::VIndent(OutputSettings.help_margin_width, ln.size()), desc));
		}
	} };

	lines.reserve(8u + modes.size() + options.size());
	lines.emplace_back("USAGE:");
	lines.emplace_back("  " + usage_str);

	lines.emplace_back("");
	lines.emplace_back("MODES:");

	compile_argument_vector(modes);

	lines.emplace_back("");
	lines.emplace_back("OPTIONS:");

	compile_argument_vector(options);

	lines.shrink_to_fit();
	return lines;
}

// Help
inline void mode_help(const std::string& usage_str, const std::vector<ArgumentPair>& modes, const std::vector<ArgumentPair>& options)
{
	for (auto& ln : compile_help_doc(usage_str, modes, options))
		std::cout << ln << '\n';
}

// Data Size Conversions
inline void mode_data(const std::vector<opt::Parameter>& params)
{
	for (auto arg{ params.begin() }; arg != params.end(); ++arg)
		std::cout << data::Conversion(arg, params.end()) << '\n';
}

// Hexadecimal Conversions
inline void mode_hex(const std::vector<opt::Parameter>& params)
{
	for (auto arg{ params.begin() }; arg != params.end(); ++arg) {
		std::cout << hex::ToDec(*arg) << '\n';
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

	if (check_args(args, HIDE_TYPES))
		OutputSettings.hide_types = true;

	if (check_args(args, NO_COLOR))
		OutputSettings.Palette._active = false;
}

// MAIN
int main(const int argc, char** argv, char** envp)
{
	try {
		std::cout << sys::term::EnableANSI << std::fixed;
		opt::ParamsAPI args{ argc, argv };
		const auto [program_path, program_name]{ opt::resolve_split_path(envp, args.arg0().value_or(argv[0])) };

		init_settings_from_args(args);

		// Check if args are empty / help arg included
		if (args.empty() || check_args(args, HELP))
			mode_help(program_name + " <MODE> [OPTIONS] [PARAMETERS]", { HELP, CONVERT_DATASIZE, CONVERT_HEXADECIMAL }, { NUMBER_GROUPING, OUTPUT_ONLY, HIDE_TYPES, NO_COLOR });

		// Get all parameters
		const auto params{ args.getAllParameters<std::vector<opt::Parameter>>() };

		// Select primary mode
		if (check_args(args, CONVERT_DATASIZE)) // convert params as data sizes
			mode_data(params);
		else if (check_args(args, CONVERT_HEXADECIMAL)) // convert params as hexadecimal/decimal
			mode_hex(params);
		
		return 0;
	} catch ( std::exception& ex ) {
		std::cout << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch ( ... ) {
		std::cout << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}