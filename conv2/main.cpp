#include "version.h"

#include <TermAPI.hpp>
#include <ParamsAPI2.hpp>
#include <palette.hpp>
#include <hasPendingDataSTDIN.h>

#include <iostream>
#include <iomanip>

inline std::ostream& print_help(std::ostream& os)
{
	return os
		<< "conv2  " << CONV2_VERSION << '\n'
		<< "  A commandline conversion utility.\n"
		<< '\n'
		<< "USAGE:\n"
		<< "  conv2 [OPTIONS] [<MODE> <<INPUT>...>]\n"
		<< '\n'
		<< "OPTIONS:\n"
		<< "  -h  --help            Show this help display, then exit." << '\n'
		<< "  -v  --version         Show the current version number, then exit." << '\n'
		<< "  -q  --quiet           Only show minimal output." << '\n'
		<< "  -g  --group           Use number grouping for large numbers. (Ex. 1,000,000)" << '\n'
		<< "  -n  --no-color        Disable the usage of colorized output." << '\n'
		<< "      --showbase        Force " << '\n'
		<< "      --precision <#>   Specify the number of digits after the decimal point to show." << '\n'
		<< "      --fixed           Force standard notation." << '\n'
		<< "      --scientific      Force scientific notation." << '\n'
		<< "      --hexfloat        Force floating-point numbers to use hexadecimal." << '\n'
		<< '\n'
		<< "MODES:\n"
		<< "  -d  --data            Data Size Conversions. (B, kB, MB, GB, etc.)" << '\n'
		<< "  -x  --hex             Hexadecimal <=> Decimal Conversions." << '\n'
		<< "  -m  --mod             Modulo Calculator." << '\n'
		<< "  -l  --len             Length Unit Conversions. (meters, feet, etc.)" << '\n'
		<< "  -a  --ascii           ASCII Table Lookup Tool. Converts all characters to their ASCII values." << '\n'
		<< "  -R  --rad             Degrees <=> Radians Converter." << '\n'
		<< "  -F  --FOV <H:V>       Horizontal <=> Vertical Field of View Converter." << '\n'
		<< "                        Requires an aspect ratio, specify one with \"H:V\". (Ex: \"-F 16:9\")" << '\n'
		;
}

#include <data.hpp>		// DATA
#include <base.hpp>		// HEX
#include <modulo.hpp>	// MODULO
#include <length.hpp>	// LENGTH
#include <ascii.hpp>	// ASCII
#include <radians.hpp>	// RADIANS
#include <FOV.hpp>		// FOV

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

int main(const int argc, char** argv)
{
	enum class OUTCOLOR : unsigned char {
		NONE,
		INPUT,
		OUTPUT,
		OPERATOR,
	};
	term::palette<OUTCOLOR> color{
		std::make_pair(OUTCOLOR::NONE, color::white),
		std::make_pair(OUTCOLOR::INPUT, color::yellow),
		std::make_pair(OUTCOLOR::OUTPUT, color::yellow),
		std::make_pair(OUTCOLOR::OPERATOR, color::white),
	};

	// use a buffer to prevent changes to standard output
	std::stringstream buffer;
	int returnCode = 1;

	try {
		// parse arguments
		opt::Args args{ argc, argv, 'F', "FOV", 'V' };

		// @brief	Lambda that checks if either the given flag or option were included on the commandline.
		const auto& checkarg{ [&args](const std::optional<char>& flag, const std::optional<std::string>& opt) {
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
		if (args.empty() || checkarg('h', "help")) {
			std::cout << print_help;
			return 0;
		}
		// [-v|--version]
		else if (checkarg('v', "version")) {
			std::cout << (quiet ? "" : "conv2  ") << CONV2_VERSION << std::endl;
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
		if (checkarg('d', "data")) {
			for (auto arg{ parameters.begin() }; arg != parameters.end(); ++arg) {
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
		else if (checkarg('x', "hex")) {
			for (const auto& it : parameters) {
				if (!quiet)
					buffer << color(OUTCOLOR::INPUT) << it << color() << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				switch (base::detect_base(it)) {
				case base::ValueBase::DECIMAL:
					buffer << color(OUTCOLOR::OUTPUT) << base::to_hex(it, std::uppercase, (numGrouping ? str::NumberGrouping : str::Placeholder), "0x") << color() << '\n';
					break;
				case base::ValueBase::HEXADECIMAL:
					buffer << color(OUTCOLOR::OUTPUT) << base::to_decimal(it) << color() << '\n';
					break;
				case base::ValueBase::INVALID: [[fallthrough]];
				default:
					throw make_exception("Invalid number: \"", it, "\"!");
				}
			}
		}
		// MODULO
		else if (checkarg('m', "mod")) {
			for (auto it{ parameters.begin() }; it != parameters.end(); ++it) {
				std::string here{ *it }, next{ "" };
				if (const auto& pos{ here.find('%') }; pos != std::string::npos) {
					next = here.substr(pos + 1ull);
					if (next.find('%'))
						throw make_exception("Cannot calculate nested modulo operators in parameters: \"", here, ' ', next, '\"');
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
		else if (checkarg('l', "len")) {
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
					const auto& [in_unit, value, out_unit] { unit::Convert(get_tuple(it))._vars };
					const auto result{ unit::Convert::getResult(in_unit, value, out_unit) };
					if (!quiet)
						buffer << color(OUTCOLOR::INPUT) << value << color() << ' ' << in_unit << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
					buffer << color(OUTCOLOR::OUTPUT) << result << color() << '\n';
				}
			}
		}
		// ASCII
		else if (checkarg('a', "ascii")) {
			for (auto it{ parameters.begin() }; it != parameters.end(); ++it) {
				if (!quiet)
					buffer << color(OUTCOLOR::INPUT) << *it << color() << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				if (std::all_of(it->begin(), it->end(), isdigit))
					buffer << color(OUTCOLOR::OUTPUT) << static_cast<char>(str::stoll(*it)) << color() << ' ';
				else for (const auto& c : *it)
					buffer << color(OUTCOLOR::OUTPUT) << static_cast<short>(c) << color() << ' ';
				buffer << '\n';
			}
		}
		// RADIANS
		else if (checkarg('R', "rad")) {
			for (std::string it : parameters) {
				const bool in_radians{ str::tolower(it).find('c') != std::string::npos };
				it.erase(std::remove_if(it.begin(), it.end(), isalpha), it.end());
				const auto v{ str::stoll(it) };
				if (!quiet) {
					buffer << color(OUTCOLOR::INPUT) << v << color() << ' ';
					if (in_radians)
						buffer << "rad";
					else
						buffer << "deg";
					buffer << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				}
				if (in_radians)
					buffer << color(OUTCOLOR::OUTPUT) << toRadians(v) << color() << ' ' << "rad" << '\n';
				else
					buffer << color(OUTCOLOR::OUTPUT) << toDegrees(v) << color() << ' ' << "deg" << '\n';
			}
		}
		// FOV
		else if (checkarg('F', "FOV")) {
			const auto& fov{ args.typegetv_any<opt::Flag, opt::Option>('F', "FOV") };
			if (!fov.has_value())
				throw make_exception("Detected mode: FOV\n", indent(10), "No aspect ratio was specified!");

			const auto& isVertical{ [](auto&& str) {
				const auto lower{ str::tolower(str) };
				if (const auto& pos{ lower.find('v') }; pos != std::string::npos)
					return true;
				if (const auto& pos{ lower.find('h') }; pos != std::string::npos)
					return false;
				throw make_exception("Cannot determine FOV orientation of \"", str, "\"!\n",
					indent(10), "Append 'H' to specify a Horizontal FOV.\n",
					indent(10), "Append 'V' to specify a Vertical FOV."
				);
			} };

			FOV::AspectRatio aspect{ 0, 0 };

			std::string captured{ fov.value() };
			if (const auto& pos{ captured.find(':') }; pos != std::string::npos) {
				aspect.h = str::stoui(captured.substr(0ull, pos));
				aspect.v = str::stoui(captured.substr(pos + 1ull));
			}
			else throw make_exception("Invalid aspect ratio specifier: \"", fov.value(), "\"!\n", indent(10), "Aspect ratios must be in the format \"Horizontal:Vertical\".");

			for (std::string it : parameters) {
				bool vertical{ isVertical(it) };
				it.erase(std::remove_if(it.begin(), it.end(), isalpha), it.end());
				if (!quiet)
					buffer << color(OUTCOLOR::INPUT) << it << color() << ' ' << (vertical ? 'V' : 'H') << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				if (vertical)
					buffer << color(OUTCOLOR::OUTPUT) << FOV::toHorizontal(str::stold(it), aspect) << color() << " H\n";
				else
					buffer << color(OUTCOLOR::OUTPUT) << FOV::toVertical(str::stold(it), aspect) << color() << " V\n";
			}
		}
		else throw make_exception("Nothing to do; no mode was specified!");

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