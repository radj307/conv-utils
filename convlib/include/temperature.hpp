#pragma once

#include <make_exception.hpp>
#include <str.hpp>

#include <algorithm>
#include <ostream>

namespace conv {
	enum class TemperatureSystem : std::int8_t {
		Kelvin = 1,
		Celcius = 2,
		Fahrenheit = 3,
	};

	inline bool operator==(const TemperatureSystem& l, const int& r)
	{
		return l == static_cast<TemperatureSystem>(r);
	}

	template<typename T>
	T convertFahrenheitToCelcius(T const value)
	{
		return static_cast<T>((static_cast<long double>(value) - 32.0L) / 1.8L);
	}
	template<typename T>
	T convertCelciusToFahrenheit(T const value)
	{//                                                         ▼▼▼▼ == (9 / 5)
		return static_cast<T>(static_cast<long double>(value) * 1.8L + 32.0L);
	}
	template<typename T>
	T convertCelciusToKelvin(T const value)
	{
		return static_cast<T>(static_cast<long double>(value) + 273.15L);
	}
	template<typename T>
	T convertKelvinToCelcius(T const value)
	{
		return static_cast<T>(static_cast<long double>(value) - 273.15L);
	}

	template<typename T>
	T convert(TemperatureSystem const inputSystem, T const value, TemperatureSystem const outputSystem)
	{
		switch (outputSystem) {
		case TemperatureSystem::Kelvin:
			switch (inputSystem) {
			case TemperatureSystem::Kelvin: return value;
			case TemperatureSystem::Celcius: return convertCelciusToKelvin(value);
			case TemperatureSystem::Fahrenheit: return convertCelciusToKelvin(convertFahrenheitToCelcius(value));
			}
			break;
		case TemperatureSystem::Celcius:
			switch (inputSystem) {
			case TemperatureSystem::Kelvin: return convertKelvinToCelcius(value);
			case TemperatureSystem::Celcius: return value;
			case TemperatureSystem::Fahrenheit: return convertFahrenheitToCelcius(value);
			}
			break;
		case TemperatureSystem::Fahrenheit:
			switch (inputSystem) {
			case TemperatureSystem::Kelvin: return convertCelciusToFahrenheit(convertKelvinToCelcius(value));
			case TemperatureSystem::Celcius: return convertCelciusToFahrenheit(value);
			case TemperatureSystem::Fahrenheit: return value;
			}
			break;
		default:
			throw make_exception("Invalid output temperature system: '", static_cast<std::int8_t>(outputSystem), "'");
		}
		throw make_exception("Invalid input temperature system: '", static_cast<std::int8_t>(inputSystem), "'");
	}

	template<typename T>
	struct temperature {
		TemperatureSystem system;
		T value;

		temperature to_system(TemperatureSystem const sys) const
		{
			return{ sys, convert<T>(system, value, sys) };
		}
	};

	/**
	 * @brief		Gets the TemperatureSystem associated with the given char representation.
	 *\n			Lowercase characters are not accepted.
	 * @param ch	The char representation of a Temperature System.
	 * @returns		When ch is 'K', TemperatureSystem::Kelvin; When ch is 'C', TemperatureSystem::Celcius; When ch is 'F', TemperatureSystem::Fahrenheit; Otherwise returns TemperatureSystem{0}.
	 */
	inline TemperatureSystem getTemperatureSystem(const char ch)
	{
		switch (ch) {
		case 'K':
			return TemperatureSystem::Kelvin;
		case 'C':
			return TemperatureSystem::Celcius;
		case 'F':
			return TemperatureSystem::Fahrenheit;
		default:
			return TemperatureSystem{ 0 };
		}
	}
	/**
	 * @brief			Gets the symbol representation associated with the given TemperatureSystem as a string.
	 * @param system	Input Temperature System.
	 * @returns			A string representing the given TemperatureSystem.
	 */
	inline std::string getTemperatureSystemSymbol(const TemperatureSystem system)
	{
		switch (system) {
		case TemperatureSystem::Kelvin:
			return " K";
		case TemperatureSystem::Celcius:
			return "°C";
		case TemperatureSystem::Fahrenheit:
			return "°F";
		default:
			return " ?";
		}
	}

	template<typename T>
	struct TempConversion {
		temperature<T> temperature_value;
		TemperatureSystem output_system;

		temperature<T> getResult() const
		{
			return temperature_value.to_system(output_system);
		}
	};

	std::vector<std::string> temperature_preprocess_arguments(std::vector<std::string> const& arguments)
	{
		std::vector<std::string> vec;
		vec.reserve(arguments.size());

		for (const auto& it : arguments) {
			if (const auto& fstAlpha{ std::find_if(it.begin(), it.end(), [](auto&& ch) { return std::isalpha(ch); }) };
				fstAlpha != it.end()) {
				if (const auto& fstDigit{ std::find_if(it.begin(), it.end(), [](auto&& ch) { return std::isdigit(ch); }) };
					fstDigit != it.end()) {
					const auto& fstAlphaIndex{ std::distance(it.begin(), fstAlpha) };
					if (fstAlpha > fstDigit) { // input value precedes input unit
						vec.emplace_back(str::trim(it.substr(fstAlphaIndex)).substr(0ull, 1ull)); //< push input unit
						vec.emplace_back(str::trim(it.substr(0ull, fstAlphaIndex))); //< push input value
						continue;
					}
					else { // input unit precedes input value
						vec.emplace_back(str::trim(it.substr(0ull, fstAlphaIndex)).substr(0ull, 1ull)); //< push input unit
						vec.emplace_back(str::trim(it.substr(fstAlphaIndex))); //< push input value
						continue;
					}
				}
			}
			vec.emplace_back(it);
		}

		return vec;
	}

	template<typename T> requires std::floating_point<T> || std::integral<T>
	std::vector<TempConversion<T>> temperature_parse_arguments(std::vector<std::string> arguments)
	{
		arguments = temperature_preprocess_arguments(arguments);

		std::vector<TempConversion<T>> vec;
		vec.reserve(arguments.size() / 2);

		std::optional<temperature<T>> input_temp{ std::nullopt };

		for (auto it{ arguments.begin() }; it != arguments.end(); ++it) {
			if (!input_temp.has_value()) {
				if (str::isnumber(*it)) { // reorder elements
					const auto value_copy{ *it };
					auto next{ it + 1 };
					*it = *next;
					*next = value_copy;
				}

				const auto sys{ getTemperatureSystem(it->at(0ull)) };
				if (sys == 0) // ERROR ; Expected a valid temperature system
					throw make_exception("'", *it, "' is not a valid temperature system!\n(Expected 'K' (Kelvin), 'C' (Celcius), or 'F' (Fahrenheit))");
				// increment iterator to next element
				if (++it == arguments.end())
					break;
				if (!str::isnumber(*it)) // ERROR ; Expected input value
					throw make_exception("'", *it, "' is not a valid number!");

				const T val{ str::tonumber<T>(*it) };

				if (sys == TemperatureSystem::Kelvin && val < 0.0f) // ERROR ; Kelvin temps cannot be negative
					throw make_exception("'", *it, "' is not a valid Kelvin temperature value because it is negative! (0 Kelvin == Absolute Zero)");

				input_temp = temperature<T>{ sys, val };
			}
			else {
				const auto sys{ getTemperatureSystem(str::toupper(it->at(0ull))) };

				if (sys == 0) // ERROR ; Expected a valid temperature system
					throw make_exception("'", *it, "' is not a valid temperature system!\n(Expected 'K' (Kelvin), 'C' (Celcius), or 'F' (Fahrenheit))");

				vec.emplace_back(TempConversion<T>{ input_temp.value(), sys });
				input_temp = std::nullopt;
			}
		}

		vec.shrink_to_fit();
		return vec;
	}
}
