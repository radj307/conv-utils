#pragma once
#include <sysarch.h>
#include <string>
#include <vector>

namespace ascii {
	using CharT = wchar_t;
	using StrT = std::wstring;
	using ValueT = int;
	using ValueCont = std::vector<ValueT>;

	WINCONSTEXPR inline ValueT to_ascii(CharT const& ch)
	{
		return static_cast<ValueT>(ch);
	}
	WINCONSTEXPR inline ValueCont to_ascii(StrT const& s)
	{
		ValueCont vec;
		vec.reserve(s.size());
		for (auto& ch : s)
			vec.emplace_back(to_ascii(ch));
		vec.shrink_to_fit();
		return vec;
	}

	WINCONSTEXPR inline CharT from_ascii(ValueT const& v)
	{
		return static_cast<CharT>(v);
	}
	WINCONSTEXPR inline StrT from_ascii(ValueCont const& vc)
	{
		StrT s;
		s.reserve(vc.size());
		for (auto& v : vc)
			s.push_back(from_ascii(v));
		s.shrink_to_fit();
		return s;
	}
}
