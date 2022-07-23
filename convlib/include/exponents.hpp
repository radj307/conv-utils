#pragma once
#include "base.hpp"

#include <str.hpp>

#include <ostream>
#include <concepts>
#include <variant>
#include <regex>

namespace exponents {

	template<typename T> concept accepted_by_pow_func = requires()
	{
		std::pow(std::declval<T>(), std::declval<T>());
	};

	struct Pow {
		std::string number, exponent;
	protected:
		static WINCONSTEXPR long long getBase10(std::string const& s)
		{
			switch (base::detectBase(s)) {
			case base::enumerator::Base::BINARY:
				return str::toBase10(s, 2);
			case base::enumerator::Base::OCTAL:
				return str::toBase10(s, 8);
			case base::enumerator::Base::DECIMAL:
				return str::stoll(s);
			case base::enumerator::Base::HEXADECIMAL:
				return str::toBase10(s, 16);
			default:
				return 0;
			}
		}
		template<accepted_by_pow_func T>
		T getResult(std::function<T(std::string)> const& converter) const
		{
			return std::pow(converter(number), converter(exponent));
		}
	public:
		WINCONSTEXPR Pow() {}
		WINCONSTEXPR Pow(std::string const& number, std::string const& exponent) : number{ number }, exponent{ exponent } {}
		WINCONSTEXPR Pow(std::string&& number, std::string&& exponent) : number{ std::move(number) }, exponent{ std::move(exponent) } {}
		/// @brief	Returns true when the number or exponent should be parsed as a floating-point number.
		bool hasFloatingPoint() const
		{
			return number.find('.') < number.size() || exponent.find('.') < exponent.size();
		}
		bool hasNegative() const
		{
			static const std::string search_str{ "-" };
			return number.find_first_of(search_str) < number.find_first_not_of(search_str) || exponent.find_first_of(search_str) < exponent.find_first_not_of(search_str);
		}
		template<std::signed_integral TReturn>
		TReturn getResult() const
		{
			return static_cast<TReturn>(this->getResult<long long>(str::stoll));
		}
		template<std::unsigned_integral TReturn>
		TReturn getResult() const
		{
			return static_cast<TReturn>(this->getResult<long long>(str::stoull));
		}
		template<std::floating_point TReturn>
		TReturn getResult() const
		{
			return static_cast<TReturn>(this->getResult<long double>(str::stold));
		}
		template<var::Streamable... Ts>
		std::string getResultString(Ts&&... stream_formatting) const
		{
			if (hasFloatingPoint())
				return str::stringify(std::forward<Ts>(stream_formatting)..., getResult<long double>());
			else if (hasNegative())
				return str::stringify(std::forward<Ts>(stream_formatting)..., getResult<long long>());
			else
				return str::stringify(std::forward<Ts>(stream_formatting)..., getResult<unsigned long long>());
		}
	};
}
