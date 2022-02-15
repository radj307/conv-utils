#include "version.h"

#include <TermAPI.hpp>
#include <ParamsAPI2.hpp>
#include <palette.hpp>

#include <iostream>

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
		<< '\n'
		<< "MODES:\n"
		<< "  -d  --data            Data Size Conversions. (B, kB, MB, GB, etc.)" << '\n'
		<< "  -x  --hex             Hexadecimal <=> Decimal Conversions." << '\n'
		<< "  -m  --mod             Modulo Calculator." << '\n'
		<< "  -l  --len             Length Unit Conversions. (meters, feet, etc.)" << '\n'
		<< "  -a  --ascii           ASCII Table Lookup." << '\n'
		<< "  -R  --rad             Degrees <=> Radians Converter." << '\n'
		<< "  -F  --FOV <H:V>       Horizontal <=> Vertical Field of View Converter." << '\n'
		<< "                        You must specify an aspect ratio with \"Horizontal:Vertical\"." << '\n'
		;
}

#include <data.hpp>		// DATA
#include <base.hpp>		// HEX
#include <modulo.hpp>	// MODULO
#include <unit.hpp>		// LENGTH
#include <ascii.hpp>	// ASCII
#include <radians.hpp>	// RADIANS
#include <FOV.hpp>		// FOV


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

	try {
		opt::ParamsAPI2 args{ argc, argv, 'F', "FOV" };

		const auto& getarg_any{ [&args](const std::optional<char>& flag, const std::optional<std::string>& opt) {
			if (flag.has_value() && opt.has_value())
				return args.typegetv_any<opt::Flag, opt::Option>(flag.value(), opt.value());
			else if (flag.has_value())
				return args.typegetv_any<opt::Flag>(flag.value());
			else if (opt.has_value())
				return args.typegetv_any<opt::Option>(opt.value());
			return static_cast<std::optional<std::string>>(std::nullopt);
		} };
		const auto& checkarg{ [&args](const std::optional<char>& flag, const std::optional<std::string>& opt) {
			if (flag.has_value() && opt.has_value())
				return args.check_any<opt::Flag, opt::Option>(flag.value(), opt.value());
			else if (flag.has_value())
				return args.check_any<opt::Flag>(flag.value());
			else if (opt.has_value())
				return args.check_any<opt::Option>(opt.value());
			return false;
		} };

		color.setActive(!checkarg('n', "no-color"));
		bool quiet{ checkarg('q',"quiet") };
		bool numGrouping{ getarg_any('g', "group") };

		if (args.empty() || checkarg('h', "help")) {
			std::cout << print_help;
			return 0;
		}
		else if (checkarg('v', "version")) {
			std::cout << (quiet ? "" : "conv2  ") << CONV2_VERSION << std::endl;
			return 0;
		}

		const auto& parameters{ args.typegetv_all<opt::Parameter>() };

		std::stringstream buffer;

		if (checkarg('d', "data")) {
			// DATA
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
		else if (checkarg('x', "hex")) {
			// HEX
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
		else if (checkarg('m', "mod")) {
			// MODULO
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
		else if (checkarg('l', "len")) {
			// LENGTH
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
					const auto [in_unit, value, out_unit] { unit::Convert(get_tuple(it))._vars };
					const auto result{ unit::Convert::getResult(in_unit, value, out_unit) };
					if (!quiet)
						buffer << color(OUTCOLOR::INPUT) << value << color() << ' ' << in_unit << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
					buffer << color(OUTCOLOR::OUTPUT) << result << color() << '\n';
				}
			}
		}
		else if (checkarg('a', "ascii")) {
			// ASCII
			for (auto it{ parameters.begin() }; it != parameters.end(); ++it) {
				if (!quiet)
					buffer << color(OUTCOLOR::INPUT) << *it << color() << ' ' << color(OUTCOLOR::OPERATOR) << '=' << color() << ' ';
				for (const auto& c : *it)
					buffer << color(OUTCOLOR::OUTPUT) << static_cast<short>(c) << color() << ' ';
				buffer << '\n';
			}
		}
		else if (checkarg('R', "rad")) {
			// RADIAN
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
		else if (const auto& fov{ getarg_any('F', "FOV") }; fov.has_value()) {
			// FOV
			const auto& isVertical{ [](auto&& str) {
				const auto lower{str::tolower(str)};
				if (const auto& pos{ lower.find('v') }; pos != std::string::npos)
					return true;
				if (const auto& pos{ lower.find('h') }; pos != std::string::npos)
					return false;
				throw make_exception("Cannot determine whether \"", str, "\" is a vertical or horizontal FOV; Append 'V' or 'H' to the value!");
			} };

			FOV::AspectRatio aspect{ 0,0 };

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
		else throw make_exception("No mode argument specified! Nothing to do.");

		std::cout << buffer.rdbuf() << std::endl;

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << color.get_error() << ex.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << color.get_error() << "An undefined exception occurred!" << std::endl;
		return 1;
	}
}