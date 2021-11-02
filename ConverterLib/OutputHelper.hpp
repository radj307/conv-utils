#pragma once
#include <ostream>
#include <concepts>
#include <cmath>
#include <utility>
#include <str.hpp>
#include <Argument.hpp>

/**
 * @enum UIElement
 * @brief Various Output UI Elements, used by the color palette to easily select output colors.
 */
enum class UIElement {
	DATA_EQUALS,
	DATA_INPUT_VALUE,
	DATA_INPUT_TYPE,
	DATA_OUTPUT_VALUE,
	DATA_OUTPUT_TYPE,
	HEX_EQUALS,
	HEX_INPUT,
	HEX_OUTPUT,
};

static struct { // Program-wide output settings
	bool number_grouping{ false };
	bool output_only{ false };
	bool hide_types{ false };

} OutputSettings;

/// @brief Print floating-point numbers with as-needed precision
template<typename T> requires std::is_floating_point_v<T>
struct FloatPrinter {
	const T _value;

	/// @brief Default Constructor. @param value - A templated floating-point value.
	constexpr FloatPrinter(T value) : _value{ std::move(value) } {}

	constexpr operator T() const { return _value; }
	constexpr operator const std::string() const
	{
		const auto str{ str::stringify(std::fixed, OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder, _value) };
		const size_t dpos{ str.find('.') }, last_pos{ str.find_last_of("123456789", dpos + 1u) };
		if (last_pos < dpos)
			return str.substr(0u, dpos);
		return str.substr(0u, dpos + last_pos);
	}

	friend std::ostream& operator<<(std::ostream& os, const FloatPrinter<T>& obj)
	{
		os << obj.operator const std::string();
		return os;
	}
};

template<typename T> requires std::is_integral_v<T>
struct IntPrinter {
	const T _value;

	constexpr IntPrinter(T value) : _value{ std::move(value) } {}

	constexpr operator T() const { return _value; }
	constexpr operator const std::string() const
	{
		return str::stringify(std::fixed, OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder, _value);
	}

	friend std::ostream& operator<<(std::ostream& os, const IntPrinter<T>& obj)
	{
		os << obj.operator const std::string();
		return os;
	}
};

/// @brief Compile the help documentation as a vector of strings where each element is one line.
inline std::vector<std::string> compile_help_doc(const std::string& usage_str, std::vector<ArgumentPair> modes, std::vector<ArgumentPair> options)
{
	std::vector<std::string> lines;

	size_t margin{ 0ull };
	for (auto& [arg, _] : modes)
		if (arg.has_opt() && arg.opt().size() > margin)
			margin = arg.opt().size();
	for (auto& [arg, _] : options)
		if (arg.has_opt() && arg.opt().size() > margin)
			margin = arg.opt().size();
	margin += 8ull;

	const auto compile_argument_vector{ [&lines, &margin](const std::vector<ArgumentPair>& vec) {
		for (auto& [arg, desc] : vec) {
			const auto ln{ str::stringify(str::stringify(arg.has_flag() ? std::string("-") + arg.flag() : "  ", "  ", arg.has_opt() ? std::string("--") + arg.opt() : "")) };
			lines.emplace_back(str::stringify("  ", ln, str::VIndent(static_cast<std::streamsize>(margin), ln.size()), desc));
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
