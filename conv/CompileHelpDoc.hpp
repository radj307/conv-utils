#pragma once
#include "Argument.hpp"

/// @brief Compile the help documentation as a vector of strings where each element is one line.
inline std::vector<std::string> compile_help_doc(const std::string& usage_str, std::vector<ArgumentPair> modes, std::vector<ArgumentPair> options)
{
	std::vector<std::string> lines;

	size_t margin{ 0ull };
	for (auto& it : modes)
		if (it.has_opt() && it.opt().size() > margin)
			margin = it.opt().size();
	for (auto& it : options)
		if (it.has_opt() && it.opt().size() > margin)
			margin = it.opt().size();
	margin += 8ull;

	const auto compile_argument_vector{ [&lines, &margin](const std::vector<ArgumentPair>& vec) {
		for (auto& it : vec) {
			const auto ln{ str::stringify(str::stringify(it.has_flag() ? std::string("-") + it.flag() : "  ", "  ", it.has_opt() ? std::string("--") + it.opt() : "")) };
			lines.emplace_back(str::stringify("  ", ln, str::VIndent(static_cast<std::streamsize>(margin), ln.size()), it._desc));
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
