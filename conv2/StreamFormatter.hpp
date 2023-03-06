#pragma once
#include <opt3.hpp>
#include <strconv.hpp>

#include <iostream>

/**
 * @struct	StreamFormatter
 * @brief	Handles output stream formatting arguments.
 */
struct StreamFormatter {
private:
	const opt3::ArgManager* args;

public:
	bool
		showbase,
		precision,
		fixed,
		scientific,
		hexfloat;

	StreamFormatter(const opt3::ArgManager* args) : args{ args },
		showbase{ args->check<opt3::Option>("showbase") },
		precision{ args->check<opt3::Option>("precision") },
		fixed{ args->check<opt3::Option>("fixed") },
		scientific{ args->check<opt3::Option>("scientific") },
		hexfloat{ args->check<opt3::Option>("hexfloat") }
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
			if (const auto& precision{ fmt.args->getv<opt3::Option>("precision") }; precision.has_value()) {
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

