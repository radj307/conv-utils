#pragma once
#include <sysarch.h>
#include <str.hpp>
#include <var.hpp>
#include <OutputHelper.hpp>

#include <utility>
#include <functional>

namespace conv {
	template<class Func, typename ReturnT, typename... Arguments>
	struct FunctionWrapper {
		const Func func;

		FunctionWrapper(const Func& function) : func{ function } {}

		ReturnT operator()(Arguments&&... args) { return func(std::forward<Arguments>(args)...); }
	};

	template<class Func, typename ReturnT, typename... Arguments>
	struct FunctionStream : public FunctionWrapper<Func, ReturnT, Arguments...> {
		std::tuple<Arguments...> args;

		FunctionStream(const Func& function, std::tuple<Arguments...>&& args) : FunctionWrapper<Func, ReturnT, Arguments...>(function), args{ std::move(args) } {}

		friend std::ostream& operator<<(std::ostream& os, const FunctionStream& fs)
		{
			return os << std::apply(fs.operator(), fs.args);
		}
	};

	template<var::arithmetic T>
	inline std::string hexconv(T&& number)
	{
		const std::string s{ str::stringify(std::setprecision(OutputSettings.precision), number) };
		return str::stringify(
			Palette.set(UIElement::HEX_INPUT),
			s,
			Palette.reset(),
			str::VIndent(6ull, s.size())
		);
	}

	template<var::arithmetic... Ts>
	inline std::string hexconv(Ts&&... numbers)
	{
		
	}
}