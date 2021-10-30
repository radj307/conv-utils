#pragma once
#include "pch.h"
#include <strmanip.hpp>
#include <binary.hpp>

inline std::vector<std::tuple<std::string, std::string>> read_resource_file(const std::string& filepath, const char delim, const size_t& expected_line_count = 100ull)
{
	using RT = std::tuple<std::string, std::string>;
	auto buffer{ file::read(filepath) };

	if (buffer.fail()) // throw if file wasn't read correctly
		throw std::exception(std::string("Failed to read file: \"" + filepath + "\"").c_str());

	std::vector<RT> vec;
	vec.reserve(expected_line_count);
	for (std::string line{}; std::getline(buffer, line, '\n');)
		if (const auto pos{ line.find(delim)}; str::pos_valid(pos) )
			vec.emplace_back(std::make_tuple(line.substr(0u, pos), line.substr(pos + 1u)));
	vec.shrink_to_fit();
	return vec;
}