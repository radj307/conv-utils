#pragma once
#include <OutputHelper.hpp>
#include <cmath>
#include <functional>
namespace modulo {
	/// @brief The Epsilon value used when comparing floating-point numbers.
	constexpr const double EPSILON{ 1e-10 };

	template<typename T>
	struct Number {
	private:
		T _value;

	public:
		Number(T value) : _value{ std::move(value) } {}
		Number() : _value{ static_cast<T>(0) } {}

		operator T() const { return _value; }

		Number<T>& operator=(const T& value)
		{
			_value = value;
			return *this;
		}
		Number<T>& operator=(const Number<T>& o)
		{
			_value = o._value;
			return *this;
		}

		bool operator==(const T& value) const requires std::integral<T> { return _value == value; }
		bool operator==(const Number<T>& o) const requires std::integral<T> { return _value == o._value; }
		bool operator==(const T& value) const requires std::floating_point<T> { return std::abs(_value - value) < static_cast<T>(EPSILON); }
		bool operator==(const Number<T>& o) const requires std::floating_point<T> { return std::abs(_value - o._value) < static_cast<T>(EPSILON); }
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		template<typename ModType>
		auto mod(const ModType& modulo_v) const requires std::floating_point<T>&& std::floating_point<ModType>
		{
			// Note: Use std::fmod... (floating-point modulo) functions, NOT std::modf... (split-floating-point) functions!
			if constexpr (std::is_same_v<T, float>)
				return std::fmodf(_value, modulo_v);
			else if constexpr (std::is_same_v<T, double>)
				return std::fmod(_value, modulo_v);
			else if constexpr (std::is_same_v<T, long double>)
				return std::fmodl(_value, modulo_v);
			static_assert(std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>, "Unrecognized floating-point type!");
		}
		template<typename ModType>
		auto mod(const ModType& modulo_v) const requires std::integral<T>&& std::integral<ModType>
		{
			return _value % modulo_v;
		}
		auto mod(const Number<T>& modulo_v) const
		{
			return mod<T>(modulo_v.operator T());
		}
		template<typename OT> requires std::convertible_to<OT, T>
		auto mod(const Number<OT>& modulo_v) const
		{
			return mod<T>(static_cast<T>(modulo_v.operator OT()));
		}
	};

	template<typename T>
	constexpr static Number<T> get_number(std::string str_num, std::function<T(std::string)> converter)
	{
		str_num.erase(std::remove(str_num.begin(), str_num.end(), ','), str_num.end());
		const bool is_negative{ str_num.front() == '-' };
		if (is_negative)
			str_num = str_num.substr(1ull);
		if (std::all_of(str_num.begin(), str_num.end(), [](auto&& ch) {return isdigit(ch) || ch == '.'; }))
			return is_negative ? -converter(str_num) : converter(str_num);
		throw std::exception(("get_number()\tInvalid string number: \""s + str_num + "\"!"s).c_str());
	}

	struct Conversion {
		using FloatNum = Number<long double>;
		using IntNum = Number<long long>;
		using VariantT = std::variant<std::monostate, FloatNum, IntNum>;

		constexpr static VariantT to_variant(std::string str)
		{
			if (str::pos_valid(str.find('%')))
				throw std::exception(("Invalid number: \""s + str + "\""s).c_str());
			if (str::pos_valid(str.find('.'))) // if a decimal point was found, calculate as a floating-point
				return get_number<long double>(str, str::stold);
			return get_number<long long>(str, str::stoll);
		}

		std::string _in_str, _mod_str;
		VariantT _in_num, _mod_num;

		Conversion(std::vector<opt::Parameter>::const_iterator& input, std::vector<opt::Parameter>::const_iterator end)
		{
			if (str::pos_valid(_mod_str.find('%')))
				throw std::exception(("Invalid syntax, received \""s + _mod_str + "\" as the modulo value."s).c_str());
			const auto in{ *input };
			// contained within a single arg:
			if (const auto pos{ in.find('%') }; str::pos_valid(pos)) {
				_in_str = in.substr(0ull, pos);
				_in_num = to_variant(_in_str);
				_mod_str = in.substr(pos + 1ull);
				_mod_num = to_variant(_mod_str);
			}
			// split between 2 args:
			else if (_in_str = in; input != end - 1ull) {
				_in_num = to_variant(_in_str);
				_mod_str = *++input;
				_mod_num = to_variant(_mod_str);
			}
			// mod value not found:
			else
				throw std::exception(("modulo::Conversion()\tCouldn't find a modulo value for input \""s + _in_str + "\"!"s).c_str());
		}

		std::string get_output_str() const
		{
			if (std::holds_alternative<FloatNum>(_in_num)) {
				if (std::holds_alternative<FloatNum>(_mod_num))
					return FloatPrinter(std::get<FloatNum>(_in_num).mod(std::get<FloatNum>(_mod_num)));
				else if (std::holds_alternative<IntNum>(_mod_num))
					return FloatPrinter(std::get<FloatNum>(_in_num).mod(std::get<IntNum>(_mod_num)));
			}
			else if (std::holds_alternative<IntNum>(_in_num)) {
				if (std::holds_alternative<FloatNum>(_mod_num))
					return IntPrinter(std::get<IntNum>(_in_num).mod(std::get<FloatNum>(_mod_num)));
				else if (std::holds_alternative<IntNum>(_mod_num))
					return IntPrinter(std::get<IntNum>(_in_num).mod(std::get<IntNum>(_mod_num)));
			}
			return{ "?" };
		}
	};
}