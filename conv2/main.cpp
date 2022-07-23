#include "version.h"

#include <TermAPI.hpp>
#include <ParamsAPI2.hpp>
#include <palette.hpp>
#include <hasPendingDataSTDIN.h>

#include <iostream>
#include <iomanip>

struct PrintHelp {
private:
	std::string _param;
public:
	PrintHelp(std::optional<std::string> const& parameter = std::nullopt) : _param{ parameter.value_or("") } {}

	friend std::ostream& operator<<(std::ostream& os, const PrintHelp& h)
	{
		if (h._param.empty()) {// unscoped help
			os << "conv2  " << CONV2_VERSION_EXTENDED << '\n'
				<< "  A commandline conversion utility.\n"
				<< '\n'
				<< "USAGE:\n"
				<< "  conv2 <MODE> [MODIFIERS] [OPTIONS] <<INPUT>...>\n"
				<< '\n'
				<< "OPTIONS:\n"
				<< "  -h, --help [MODE]       Show this help display, then exit. Optionally, you can include the name of a mode to show" << '\n'
				<< "                           more detailed usage information about it. Mode names are case sensitive." << '\n'
				<< "  -v, --version           Show the current version number, then exit." << '\n'
				<< "  -q, --quiet             Only show minimal output." << '\n'
				<< "  -g, --group             Use number grouping for large numbers. (Ex. 1,000,000)" << '\n'
				<< "  -n, --no-color          Disable the usage of colorized output." << '\n'
				<< "      --showbase          Force-show bases for numbers." << '\n'
				<< "      --precision <#>     Specify the number of digits after the decimal point to show." << '\n'
				<< "      --fixed             Force standard notation." << '\n'
				<< "      --scientific        Force scientific notation." << '\n'
				<< "      --hexfloat          Force floating-point numbers to use hexadecimal." << '\n'
				<< '\n'
				<< "MODES:\n"
				<< "  -d, --data              Data Size Conversions. (B, kB, MB, GB, etc.)" << '\n'
				<< "  -x, --hex               Hexadecimal <=> Decimal Conversions." << '\n'
				<< "  -B, --base              Number representation base conversions. (Binary, Octal, Decimal, Hexadecimal)" << '\n'
				<< "  -m, --mod               Modulo Calculator." << '\n'
				<< "  -l, --len               Length Unit Conversions. (meters, feet, etc.)" << '\n'
				<< "  -a, --ascii             ASCII Table Lookup Tool. Converts all characters to their ASCII values." << '\n'
				<< "  -R, --rad               Degrees <=> Radians Converter." << '\n'
				<< "  -F, --FOV <H:V>         Horizontal <=> Vertical Field of View Converter. Requires an aspect ratio, ex: \"16:9\"." << '\n'
				<< "  -b, --bitwise           Perform bitwise calculations on binary, decimal, and/or hexadecimal numbers." << '\n'
				<< "  -e, --exp, --pow        Exponent Calculator.  Use a comma ',' (shell) or semicolon ';' (string) between expressions." << '\n'
				;
		}
		else { // scoped help
			std::string subject{ h._param };
			subject.erase(std::remove(subject.begin(), subject.end(), '-'), subject.end());

			// temporary buffer
			std::stringstream buffer;
			buffer << "MODE:\n";
			if (str::equalsAny(subject, "d", "data")) {
				buffer
					<< "  -d  --data              Data Size Conversions. (B, kB, MB, GB, etc.)" << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-d|--data> <<INPUT_UNIT> <VALUE> <OUTPUT_UNIT>>..." << '\n'
					<< "                    <<VALUE> <INPUT_UNIT> <OUTPUT_UNIT>>..." << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< '\n'
					<< "  Units can be specified with their symbol (ex: B, kB, MB, etc.) or full names (ex: byte, kilobyte, megabyte, etc.)." << '\n'
					<< "  Unit symbols are case-sensitive while full names are case-insensitive." << '\n'
					;
			}
			else if (str::equalsAny(subject, "x", "hex", "hexadecimal")) {
				buffer
					<< "  -x  --hex               Hexadecimal <=> Decimal Conversions." << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-x|--hex> <VALUE>..."
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< "  All decimal inputs are converted to hexadecimal, and vice-versa." << '\n'
					<< '\n'
					<< "  Inputs are treated as hexadecimal if any of the following is true:" << '\n'
					<< "    - It contains at least one alphabetic character in the range [A - F]. (case-insensitive)" << '\n'
					<< "    - It is prefixed by \"0x\"." << '\n'
					<< "  If neither of the above are true for an input, it is assumed to be in base-10." << '\n'
					;
			}
			else if (str::equalsAny(subject, "B", "base")) {
				buffer
					<< "  -B  --base              Number representation base conversions. (Binary, Octal, Decimal, Hexadecimal)" << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-B|--base> < <<BASE>:<INPUT>> <BASE> >" << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< ""
					;
			}
			else if (str::equalsAny(subject, "m", "mod", "modulo")) {
				buffer
					<< "  -m  --mod               Modulo Calculator." << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-m|--mod> <<NUMBER> <MOD>>..." << '\n'
					<< "                   <<NUMBER>%<MOD>>..." << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< "  Inputs can either be in the format \"<NUMBER> <MOD>\" or without spaces as \"<NUMBER>%<MOD>\"."
					;
			}
			else if (str::equalsAny(subject, "l", "len", "length")) {
				buffer
					<< "  -l  --len               Length Unit Conversions. (meters, feet, etc.)" << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-l|--len> <<INPUT_UNIT> <VALUE> <OUTPUT_UNIT>>..." << '\n'
					<< "                   <<VALUE> <INPUT_UNIT> <OUTPUT_UNIT>>..." << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< "  Each conversion always uses 3 parameters, so the total number of parameters must be a multiple of 3." << '\n'
					<< '\n'
					<< "  Units can be specified with their symbol (ex: ft, m, Mm, etc.) or full names (ex: feet/foot, meter, megameter, etc.)." << '\n'
					<< "  Unit symbols are case-sensitive while full names are case-insensitive." << '\n'
					<< "  Both the American spelling \"meter\" and the British spelling \"metre\" are accepted." << '\n'
					;
			}
			else if (str::equalsAny(subject, "a", "asc", "ascii")) {
				buffer
					<< "  -a  --ascii             ASCII Table Lookup Tool. Converts all characters to their ASCII values." << '\n'
					<< '\n'
					<< "MODIFIERS:\n"
					<< "  -N  --numeric           Force parameters that are entirely composed of digits to be" << '\n'
					<< "                           converted to their numerical ASCII values." << '\n'
					<< "  -s  --signed            Use signed range [-127 - 127] instead of unsigned range [0 - 255]" << '\n'
					<< "                           when interpreting input values and printing output values." << '\n'
					<< "      --linear            Print each conversion on a new line instead of using the table-style output." << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-a|--ascii> [-N|--numeric] [-u|--unsigned] [--linear] <INPUT>..." << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< '\n'
					<< "  By default, all inputs are converted to their numerical ASCII values -- including numbers." << '\n'
					<< "  This behavior can be disabled with the \"--output-text\" modifier which changes this behavior" << '\n'
					<< "  so that any entirely-numerical parameters in the range [-127 - 255] are" << '\n'
					<< "  converted to their textual representations." << '\n'
					;
			}
			else if (str::equalsAny(subject, "R", "rad", "radian", "radians")) {
				buffer
					<< "  -R  --rad               Degrees <=> Radians Converter." << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-R|--rad> <<NUMBER>[c]>..." << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< '\n'
					<< "  Inputs are assumed to be in Degrees unless the letter 'c' or 'r' is appended to them." << '\n'
					;
			}
			else if (str::equalsAny(subject, "F", "FOV")) {
				buffer
					<< "  -F  --FOV <H:V>         Horizontal <=> Vertical Field of View Converter. Requires an aspect ratio, ex: \"16:9\"." << '\n'
					<< '\n'
					<< "MODIFIERS:\n"
					<< "  -R  --rad               Use radians instead of degrees for input and output values." << '\n'
					<< "  -r  --round             Rounds the resulting output to the nearest integer." << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-F|--FOV> <<AspectHorizontal>:<AspectVertical>> <<INPUT>[H|V] ...>" << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< '\n'
					<< "  Inputs may have an orientation specifier; either a 'V' for Vertical or 'H' for Horizontal." << '\n'
					<< "  You can append the orientation specifier character to the number." << '\n'
					<< "  If no orientation is specified, Horizontal is used by default." << '\n'
					;
			}
			else if (str::equalsAny(subject, "b", "bitwise")) {
				buffer
					<< "  -b  --bitwise           Perform bitwise calculations on binary, decimal, and/or hexadecimal numbers." << '\n'
					<< '\n'
					<< "MODIFIERS:\n"
					<< "      --binary            Print output values in binary (base-2) instead of decimal." << '\n'
					<< "  -O  --octal             Print numbers in octal (base-8) instead of decimal." << '\n'
					<< "  -x  --hex               Print numbers in hexadecimal (base-16) instead of decimal." << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  conv2 <-b|--bitwise> [MODIFIER] '<NUMBER> <OPERATOR> <NUMBER>'" << '\n'
					<< '\n'
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< "  Note that bitwise expressions must be delimited with a comma (,) or semicolon (;) when using multiple" << '\n'
					<< "   expressions in the same command." << '\n'
					<< "  Nesting operations is fully supported, you can use parenthesis '()' to control order-of-operations." << '\n'
					<< '\n'
					<< "BITWISE SYNTAX:\n"
					<< "  Most operations are composed of two input values (operands) and an operator. The only exception to this rule is" << '\n'
					<< "   the 'NOT'/'~' operator, which requires only one operand." << '\n'
					<< "  Each expression is composed of an operator, and two input values; Input values are assumed to be represented" << '\n'
					<< "   in base-10 (decimal), unless prefixed with '0b' for base-2 (binary), or '0x' for base-16 (hexadecimal)." << '\n'
					<< "  Example of the base-10 representation of `60` in binary and hex, using their respective prefixes:" << '\n'
					<< '\n'
					<< "       `0b111100`" << '\n'
					<< "       `0x3C`" << '\n'
					<< '\n'
					<< "  The operator may be specified using literal operator names ( 'AND', 'OR', 'XOR', 'NOT' ), or the" << '\n'
					<< "   standard symbols ( | ^ & ~ ). Most symbols must be escaped when used directly in the shell." << '\n'
					<< "  This behavior is designed to support shell pipe operators, for example by using the `cat`" << '\n'
					<< "   or `echo` commands in combination with the '|' pipe operator like so:" << '\n'
					<< '\n'
					<< "       `cat \"file\" | conv2 -bx`" << '\n'
					;
			}
			else if (str::equalsAny(subject, "e", "exp", "pow")) {
				buffer
					<< "  -e, --exp, --pow        Exponent Calculator." << '\n'
					<< '\n'
					<< "USAGE:\n"
					<< "  Any uncaptured commandline parameters are used as input." << '\n'
					<< "  Note that each successive expression must be seperated from the previous expression with a comma (,) when used" << '\n'
					<< "   directly from the shell, or a semicolon (;) when enclosed by quotes (in most shells)." << '\n'
					<< "  Nesting operations is fully supported, you can use parenthesis '()' to control order-of-operations." << '\n'
					<< '\n'
					<< "EXPONENT SYNTAX:\n"
					<< "  Use a caret symbol (^) to seperate the exponent from the variable." << '\n'
					<< "  Seperate multiple expressions with a comma (,) or semicolon (;)." << '\n'
					<< "  To calculate the expression '5 to the power of 25 to the power of 2.', you would use:"
					<< '\n'
					<< "    5 ^ (25 ^ 2)" << '\n'
					;
			}
			else throw make_exception("Unrecognized help subject: \"", h._param, "\"!");
			os << buffer.rdbuf();
		}
		return os;
	}
};

#include <data.hpp>			// DATA
#include <base.hpp>			// HEX
#include <modulo.hpp>		// MODULO
#include <length.hpp>		// LENGTH
#include <ascii.hpp>		// ASCII
#include <radians.hpp>		// RADIANS
#include <FOV.hpp>			// FOV
#include <bitwise.hpp>		// BITWISE
#include <arithmetic.hpp>	// ARITHMETIC
#include <exponents.hpp>	// POW / EXP

#include "operators.hpp"

/**
 * @struct	StreamFormatter
 * @brief	Handles output stream formatting arguments.
 */
struct StreamFormatter {
	const opt::ParamsAPI2* args;
	bool
		showbase,
		precision,
		fixed,
		scientific,
		hexfloat;
	StreamFormatter(const opt::ParamsAPI2* args) : args{ args },
		showbase{ args->check<opt::Option>("showbase") },
		precision{ args->check<opt::Option>("precision") },
		fixed{ args->check<opt::Option>("fixed") },
		scientific{ args->check<opt::Option>("scientific") },
		hexfloat{ args->check<opt::Option>("hexfloat") }
	{}

	/**
	 * @brief		Apply stream formatting flags to the given output stream.
	 * @param os	(implicit) Output Stream Reference.
	 * @param fmt	(implicit) StreamFormatter Instance.
	 * @returns		std::ostream&
	 */
	friend std::ostream& operator<<(std::ostream& os, const StreamFormatter& fmt)
	{
		// SHOWBASE
		if (fmt.showbase)
			os << std::showbase;

		// PRECISION
		if (fmt.precision) {
			if (const auto& precision{ fmt.args->typegetv<opt::Option>("precision") }; precision.has_value()) {
				if (const auto& value{ precision.value() }; std::all_of(value.begin(), value.end(), isdigit))
					os << std::setprecision(str::stoll(value));
				else throw make_exception("\"", value, "\" isn't a valid integer!");
			}
			else throw make_exception("\"--precision\" requires an integer to specify the decimal precision!");
		}

		// NOTATIONS
		if ((fmt.fixed & fmt.scientific & fmt.hexfloat) != 0) // make sure only one notation arg was set
			throw make_exception("Cannot specify multiple notation arguments! (--fixed, --scientific, --hexfloat)");

		if (fmt.fixed)
			os << std::fixed;
		else if (fmt.scientific)
			os << std::scientific;
		else if (fmt.hexfloat)
			os << std::hexfloat;

		return os;
	}
};

/**
 * @struct	argument_exception
 * @brief	Exception type that appends the default help display to the exception message before returning.
 */
struct argument_exception : public ex::except {
	using ex::except::except;
	virtual void format() const noexcept override
	{
		std::string help{ str::stringify('\n', std::move(PrintHelp())) };

		auto* msg{ get_message() }; // allocate more memory & append help:
		msg->reserve(msg->size() + 1ull + help.size());
		msg->append(help.c_str());
	}
};

int main(const int argc, char** argv)
{
	using conv2::OUTCOLOR;
	using conv2::color;

	// use a buffer to prevent changes to standard output
	std::stringstream buffer;
	int returnCode = 1;

	try {
		// parse arguments
		opt::Args args{ argc, argv, 'h', "help", 'F', "FOV", 'V' };

		// @brief	Lambda that checks if either the given flag or option were included on the commandline.
		const auto& checkarg{ [&args](const std::optional<char>& flag, const std::optional<std::string>& opt, const bool& require_first = false) {
			if (require_first) {
				if (!args.empty()) {
					const auto& fst{ args.at(0ull) };
					if (const auto& f = std::get_if<opt::Flag>(&fst))
						return flag.has_value() && flag.value() == f->flag();
					else if (const auto& o = std::get_if<opt::Option>(&fst))
						return opt.has_value() && opt.value() == o->name();
				}
				return false;
			}
			if (flag.has_value() && opt.has_value())
				return args.check_any<opt::Flag, opt::Option>(flag.value(), opt.value());
			else if (flag.has_value())
				return args.check_any<opt::Flag>(flag.value());
			else if (opt.has_value())
				return args.check_any<opt::Option>(opt.value());
			return false;
		} };

		// handle blocking arguments
		color.setActive(!checkarg('n', "no-color"));
		bool quiet{ checkarg('q', "quiet") };
		bool numGrouping{ checkarg('g', "group") };

		// [-h|--help]
		if (args.empty() || checkarg('h', "help"))
			throw make_custom_exception<argument_exception>("No arguments were specified!");
		// [-v|--version]
		else if (checkarg('v', "version")) {
			std::cout << (quiet ? "" : "conv2  v") << CONV2_VERSION << std::endl;
			return 0;
		}

		std::vector<std::string> parameters;
		if (hasPendingDataSTDIN()) {
			const size_t& expand_by{ parameters.size() * 2 };
			parameters.reserve(parameters.size() + expand_by);
			std::string s;
			while (std::cin >> s) {
				if (s.empty())
					continue;
				parameters.emplace_back(s);
				if (parameters.size() >= parameters.capacity() - 1ull)
					parameters.reserve(parameters.size() + expand_by);
			}
		}
		for (const auto& it : args.typegetv_all<opt::Parameter>())
			parameters.emplace_back(it);

		StreamFormatter streamfmt{ &args };
		buffer << streamfmt;

		// DATA
		if (checkarg('d', "data", true)) {
			for (std::vector<std::string>::const_iterator arg{ parameters.begin() }; arg != parameters.end(); ++arg) {
				if (const auto conv{ data::Conversion(arg, parameters.end()) }; conv._in.has_value() && conv._out.has_value()) {
					if (!quiet) { // print input values
						const auto in{ conv._in.value().get() };
						buffer
							<< color(OUTCOLOR::INPUT) << in->_value << color()
							<< ' ' << in->_type
							<< color(OUTCOLOR::OPERATOR) << " = " << color();
					}
					const auto out{ conv._out.value().get() };
					buffer
						<< color(OUTCOLOR::OUTPUT) << out->_value << color()
						<< ' ' << out->_type << '\n';
				}
			}
		}
		// HEX
		else if (checkarg('x', "hex", true)) {
			for (const auto& it : parameters) {
				if (!quiet)
					buffer << color(OUTCOLOR::INPUT) << it << color() << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				switch (base::detectBase(it, Base::DECIMAL | Base::HEXADECIMAL)) {
				case Base::DECIMAL:
					buffer << color(OUTCOLOR::OUTPUT) << "0x" << str::fromBase10(it, 16) << color() << '\n';
					break;
				case Base::HEXADECIMAL:
					buffer << color(OUTCOLOR::OUTPUT) << str::toBase10(it, 16) << color() << '\n';
					break;
				case Base::ZERO: [[fallthrough]];
				default:
					throw make_exception("Invalid number: \"", it, "\"!");
				}
			}
		}
		// BASE
		else if (checkarg('B', "base")) {
			const auto& splitArg{ [](const std::string& str) -> std::pair<int, std::string> {
				int base{ 10 };
				std::string value{ 0ll };
				if (const auto& pos{ str.find(':') }; pos != std::string::npos) {
					base = str::stoi(str.substr(0ull, pos));
					value = str.substr(pos + 1ull);
				}
				else value = str;
				return{ base, value };
			} };

			std::pair<int, std::string> in;
			int outBase{ 10 };

			const auto& calculate{ [&in, &outBase]() {

			} };

			for (auto it{ parameters.begin() }; it != parameters.end(); ++it) {

			}
		}
		// MODULO
		else if (checkarg('m', "mod", true)) {
			for (auto it{ parameters.begin() }; it != parameters.end(); ++it) {
				std::string here{ *it }, next{ "" };
				if (const auto& pos{ here.find('%') }; pos != std::string::npos) {
					next = here.substr(pos + 1ull);
					here = here.substr(0ull, pos);
				}
				else if (std::distance(it, parameters.end()) >= 1ll) {
					next = *++it;
				}
				else std::cerr << color.get_warn() << "Unmatched value: \"" << here << '\"' << std::endl;
				if (!quiet)
					buffer << color(OUTCOLOR::INPUT) << here << color() << ' ' << color(OUTCOLOR::OPERATOR) << '%' << color() << ' ' << color(OUTCOLOR::INPUT) << next << color() << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				buffer << color(OUTCOLOR::OUTPUT);
				switch (modulo::find_num_type(here, next)) {
				case modulo::NumberType::FLOAT:
					buffer << modulo::Calculate(str::stold(here), str::stold(next)).getResult();
					break;
				case modulo::NumberType::INT:
					buffer << modulo::Calculate(str::stoll(here), str::stoll(next)).getResult();
					break;
				}
				buffer << color() << '\n';
			}
		}
		// LENGTH
		else if (checkarg('l', "len", true)) {
			const auto& is_value{ [](const std::string_view& str) -> bool {
				return std::all_of(str.begin(), str.end(), [](auto&& c) {return isdigit(c) || c == '.' || c == '-'; });
			} };
			const auto& get_tuple{ [&is_value](auto&& it) {
				const auto fst{ *it };
				const auto snd{ *++it };
				const auto thr{ *++it };
				if (is_value(snd))
					return std::make_tuple(fst, snd, thr);
				else return std::make_tuple(snd, fst, thr);
			} };
			for (auto it{ parameters.begin() }; it != parameters.end(); ++it) {
				if (std::distance(it, parameters.end()) >= 2ll) {
					const auto& [in_unit, value, out_unit] { length::Convert(get_tuple(it))._vars };
					const auto result{ length::Convert::getResult(in_unit, value, out_unit) };
					if (!quiet)
						buffer << color(OUTCOLOR::INPUT) << value << color() << ' ' << in_unit << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
					buffer << color(OUTCOLOR::OUTPUT) << result << color() << '\n';
				}
			}
		}
		// ASCII
		else if (checkarg('a', "ascii", true)) {
			const bool&
				disallowReverseConversion{ checkarg('N', "numeric") },
				signedRange{ checkarg('s', "signed") },
				onePerLine{ checkarg(std::nullopt, "linear") };

			for (auto it{ parameters.begin() }; it != parameters.end(); ++it) {
				// Allow Reverse Lookup:
				if (!disallowReverseConversion && std::all_of(it->begin(), it->end(), isdigit)) {
					if (!quiet)
						buffer << color(OUTCOLOR::INPUT) << *it << color() << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
					int n{ str::stoi(*it) };
					// loopback
					if (n > 127) n = -127 + n % 127;
					const char c{ static_cast<char>(n) };
					buffer << color(OUTCOLOR::OUTPUT) << c << color() << ' ';
				}
				// One Per Line Mode:
				else if (onePerLine) {
					for (const auto& c : *it) {
						if (!quiet)
							buffer << color(OUTCOLOR::INPUT) << c << color() << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
						buffer << color(OUTCOLOR::OUTPUT) << (signedRange ? static_cast<signed short>(static_cast<signed char>(c)) : static_cast<unsigned short>(static_cast<unsigned char>(c))) << color() << '\n';
					}
				}
				// Table Mode:
				else if (!quiet) {
					const size_t len{ it->size() };
					std::vector<std::string> output;
					output.reserve(len);
					buffer << color(OUTCOLOR::OPERATOR) << '{' << color() << ' ';
					for (const auto& c : *it) {
						const auto& out{ std::to_string((signedRange ? static_cast<signed short>(static_cast<signed char>(c)) : static_cast<unsigned short>(static_cast<unsigned char>(c)))) };
						output.emplace_back(out);
						buffer << color(OUTCOLOR::INPUT) << c << color() << indent(out.size() + 1ull);
					}
					buffer << color(OUTCOLOR::OPERATOR) << '}' << color() << '\n' << color(OUTCOLOR::OPERATOR) << '{' << color() << ' ';

					for (size_t i{ 0ull }, vecLen{ output.size() }; i < len && i < vecLen; ++i)
						buffer << color(OUTCOLOR::OUTPUT) << output.at(i) << color() << ' ';
					buffer << color(OUTCOLOR::OPERATOR) << '}' << color();
				}
				// Quiet Non-Linear Mode:
				else for (const auto& c : *it)
					buffer << color(OUTCOLOR::OUTPUT) << (signedRange ? static_cast<signed short>(static_cast<signed char>(c)) : static_cast<unsigned short>(static_cast<unsigned char>(c))) << color() << ' ';
				if (!onePerLine) buffer << '\n';
			}
		}
		// RADIANS
		else if (checkarg('R', "rad", true)) {
			for (const auto& it : parameters) {
				std::string lower{ str::tolower(it) };

				const bool in_radians{ str::endsWith(lower, 'c') || str::endsWith(lower, 'r') || str::endsWith(lower, "rad") };
				lower.erase(std::remove_if(lower.begin(), lower.end(), isalpha), lower.end());

				const auto v{ str::stold(lower) };
				if (!quiet) {
					buffer << color(OUTCOLOR::INPUT) << v << color() << ' ';
					if (in_radians)
						buffer << "rad";
					else
						buffer << "deg";
					buffer << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				}
				if (in_radians)
					buffer << color(OUTCOLOR::OUTPUT) << toDegrees(v) << color() << ' ' << "deg" << '\n';
				else
					buffer << color(OUTCOLOR::OUTPUT) << toRadians(v) << color() << ' ' << "rad" << '\n';
			}
		}
		// FOV
		else if (checkarg('F', "FOV", true)) {
			const auto& fov{ args.typegetv_any<opt::Flag, opt::Option>('F', "FOV") };
			const bool& radians{ checkarg('R', "rad") }, & round{ checkarg('r', "round") };

			if (!fov.has_value())
				throw make_exception("Detected mode: FOV\n", indent(10), "No aspect ratio was specified!");

			FOV::AspectRatio aspect{ 0, 0 };

			std::string captured{ fov.value() };
			if (const auto& pos{ captured.find(':') }; pos != std::string::npos) {
				aspect.h = str::stoui(captured.substr(0ull, pos));
				aspect.v = str::stoui(captured.substr(pos + 1ull));
			}
			else throw make_exception("Invalid aspect ratio specifier: \"", fov.value(), "\"!\n", indent(10), "Aspect ratios must be in the format \"Horizontal:Vertical\".");

			for (std::string it : parameters) {
				bool vertical{ str::endsWith(str::toupper(it), 'V') };
				it.erase(std::remove_if(it.begin(), it.end(), isalpha), it.end());
				if (!quiet)
					buffer << color(OUTCOLOR::INPUT) << it << color() << (radians ? " rad" : "") << ' ' << (vertical ? 'V' : 'H') << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';

				FOV::value const& in{ str::stold(it) };

				FOV::value out{ 0.0L };

				if (radians) {
					if (vertical)
						out = FOV::toHorizontalR(in, aspect);
					else
						out = FOV::toVerticalR(in, aspect);
					buffer << color(OUTCOLOR::OUTPUT) << (round ? std::round(out) : out) << color() << " rad" << ' ' << (vertical ? 'H' : 'V') << '\n';
				}
				else {
					if (vertical)
						out = FOV::toHorizontal(in, aspect);
					else
						out = FOV::toVertical(in, aspect);
					buffer << color(OUTCOLOR::OUTPUT) << (round ? std::round(out) : out) << color() << ' ' << (vertical ? 'H' : 'V') << '\n';
				}
			}
		}
		// BITWISE
		else if (checkarg('b', "bitwise", true)) {

			bool binary{ false }; // no fmtflag for binary
			std::ios_base& (*fmtFunction)(std::ios_base&) = &std::dec;
			if (checkarg('O', "octal"))
				fmtFunction = &std::oct;
			else if (checkarg('x', "hex"))
				fmtFunction = &std::hex;
			else if (checkarg('B', "binary"))
				binary = true;

			for (const auto& expr : [/*&valid_operand, &valid_operator, &match_cfg*/](auto&& params) {
				std::vector<std::string> vec;
					vec.reserve(params.size());
					std::string buf;
					buf.reserve(64ull);
					for (auto it{ params.begin() }; it != params.end(); ++it) {
						if (str::endsWithAny(*it, ',', ';')) {
							vec.emplace_back(buf + it->substr(0ull, it->size() - 2ull));
								buf.clear();
						}
						else if (std::distance(it, params.end()) == 1) {
							vec.emplace_back(buf + *it);
							buf.clear();
						}
						//else if (const bool has_operator{ std::regex_match(*it, valid_operator, match_cfg) }, has_operand{ std::regex_match(*it, valid_operand, match_cfg) }; has_operator && has_operand) { // whole expression
						//	vec.emplace_back(*it);
						//}
						else buf += *it;
					}
				vec.shrink_to_fit();
				return vec;
			}(parameters)) {
				bitwise::operation oper{ bitwise::parse(expr) };
				if (!quiet)
					buffer << oper << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				buffer << color(OUTCOLOR::OUTPUT);
				if (binary)
					buffer << str::fromBase10(oper.result(), 2);
				else
					buffer << fmtFunction << oper.result();
				buffer << color() << '\n';
			}
		}
		// EXP / POW
		else if (checkarg('e', "exp", true) || checkarg(std::nullopt, "pow", true)) {

			std::stringstream ss;
			if (hasPendingDataSTDIN())
				ss << std::cin.rdbuf();
			if (const auto& params{ args.typegetv_all<opt::Parameter>() }; !params.empty())
				ss << str::join(params, ' ');
			const auto& expressions{ str::split_all(ss.str(), ",;") };

			if (expressions.empty())
				throw make_exception("No exponent expressions were specified!");

			for (const auto& expr : expressions)
				std::cout << exponents::getOperationResult(expr, quiet).first << std::endl;
		}
		else throw make_custom_exception<argument_exception>("Nothing to do; no mode was specified!");

		returnCode = 0;
	} catch (const std::exception& ex) {
		std::cerr << color.get_error() << ex.what() << std::endl;
	} catch (...) {
		std::cerr << color.get_error() << "An undefined exception occurred!" << std::endl;
	}

	// print buffer before exit
	std::cout << buffer.rdbuf() << std::endl;

	return returnCode;
}