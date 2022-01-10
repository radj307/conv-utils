#pragma once
#include <OutputHelper.hpp>
#include <cmath>
namespace modulo {
	using FloatT = long double;
	using IntT = long long;
	template<typename T>
	concept numtype = std::same_as<T, IntT> || std::is_same_v<T, FloatT>;

	enum class NumberType : char {
		FLOAT,
		INT,
	};

	template<class... VT> requires std::conjunction_v<std::is_same<VT, std::string>...>
	inline NumberType find_num_type(const VT&... n)
	{
		if (var::variadic_or(str::pos_valid(n.find('.'))...))
			return NumberType::FLOAT;
		return NumberType::INT;
	}

	/**
	 * @struct Calculate
	 * @brief Calculate a modulo operation on integers or floating-points.
	 * @tparam T	- Allows long long or long double.
	 */
	template<numtype T> struct Calculate {
		T in, mod;

		Calculate(T input_number, T modulo_number) : in{ std::move(input_number) }, mod{ std::move(modulo_number) } {}
		/**
		 * @brief Get the resulting value from the 2 given inputs.
		 * @returns T
		 */
		T getResult() const
		{
			if constexpr (std::same_as<T, FloatT>)
				return fmodl(in, mod);
			else if constexpr (std::same_as<T, IntT>)
				return (in % mod);
			return static_cast<T>(0);
		}

		T operator()() const { return getResult(); }
	};
}