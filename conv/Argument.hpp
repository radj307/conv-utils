#pragma once
#include <ParamsAPI2.hpp>

#pragma region ARGUMENT_OBJECTS
struct ArgumentPair;
/// @brief Represents a commandline argument, which can take the form of an option, flag, or both.
struct Argument {
private:
	const std::optional<std::string> _opt;
	const std::optional<char> _flag;
public:
	/**
	 * @brief Constructor that accepts both an option name and a flag.
	 * @param opt	- Name of the option (double-dash prefix, full word) used to specify this argument.
	 * @param flag	- Name of the flag (single-dash prefix, single-character) used to specify this argument.
	 */
	constexpr Argument(std::string opt, char flag) : _opt{ std::move(opt) }, _flag{ std::move(flag) } {}
	/**
	 * @brief Constructor that accepts an option name.
	 * @param opt	- Name of the option (double-dash prefix, full word) used to specify this argument.
	 */
	constexpr Argument(std::string opt) : _opt{ std::move(opt) }, _flag{ std::nullopt } {}
	/**
	 * @brief Constructor that accepts a flag.
	 * @param flag	- Name of the flag (single-dash prefix, single-character) used to specify this argument.
	 */
	constexpr Argument(char flag) : _opt{ std::nullopt }, _flag{ std::move(flag) } {}

	bool has_opt() const
	{
		return _opt.has_value();
	}
	bool has_flag() const
	{
		return _flag.has_value();
	}

	std::string opt() const
	{
		return _opt.value();
	}
	char flag() const
	{
		return _flag.value();
	}

	std::string opt_or(std::string def) const
	{
		return _opt.value_or(def);
	}
	char flag_or(char def) const
	{
		return _flag.value_or(def);
	}

	constexpr explicit operator const std::string() const { return _opt.value(); }
	constexpr explicit operator const char() const { return _flag.value(); }
};
/// @brief Represents an argument, paired with a short usage description for the help display.
struct ArgumentPair : public Argument {
	const std::string _desc;

	/**
	 * @brief Constructor that accepts both an option name, a flag, and a description for the help display.
	 * @param opt_name		- Name of the option (double-dash prefix, full word) used to specify this argument.
	 * @param flag_name		- Name of the flag (single-dash prefix, single-character) used to specify this argument.
	 * @param description	- Brief description of what this argument does & how to use it.
	 */
	constexpr ArgumentPair(std::string opt_name, char flag_name, std::string description) : Argument{ std::move(opt_name), std::move(flag_name) }, _desc{ std::move(description) } {}
	/**
	 * @brief Constructor that accepts an option name & a description for the help display.
	 * @param opt_name		- Name of the option (double-dash prefix, full word) used to specify this argument.
	 * @param description	- Brief description of what this argument does & how to use it.
	 */
	constexpr ArgumentPair(std::string opt_name, std::string description) : Argument{ std::move(opt_name) }, _desc{ std::move(description) } {}
	/**
	 * @brief Constructor that accepts a flag & a description for the help display.
	 * @param flag_name		- Name of the flag (single-dash prefix, single-character) used to specify this argument.
	 * @param description	- Brief description of what this argument does & how to use it.
	 */
	constexpr ArgumentPair(char flag_name, std::string description) : Argument{ std::move(flag_name) }, _desc{ std::move(description) } {}

	friend std::ostream& operator<<(std::ostream& os, const ArgumentPair& argpr)
	{
		os << argpr._desc;
		return os;
	}
};
#pragma endregion ARGUMENT_OBJECTS


// define arguments
inline const ArgumentPair
MODE_HELP{ "help", 'h', "Show this argument help display" },
MODE_DATA{ "data", 'd', "Convert between digital data units. (B/kB/MB/GB/TB/PB/EB/ZB/YB)" },
MODE_HEX{ "hex", 'x', "Convert between hexadecimal and decimal digits." },
MODE_MOD{ "mod", '%', "Calculate the result of a modulo equation." },
NUMBER_GROUPING{ "number-grouping", 'g', "Insert commas when printing large numbers. (\"1000000\" => \"1,000,000\")" },
PRECISION{ "precision", "Specify the number of digits to show after the decimal point." },
OUTPUT_ONLY{ "output-only", 'o', "Show only the output values & types when printing results." },
HIDE_TYPES{ "no-type", 't', "Hides value types where applicable." },
NO_COLOR{ "no-color", "Disables colorized output." };

inline bool check_args(opt::ParamsAPI2& inst, const Argument& arg)
{
	return (arg.has_flag()
		? inst.check<opt::Flag>(arg.operator const char())
		: false)
		|| (arg.has_opt()
			? inst.check<opt::Option>(arg.operator const std::string())
			: false);
}
