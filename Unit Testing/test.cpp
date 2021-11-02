#include "pch.h"

#pragma region data
#include "data_Test.hpp"
using namespace data::unittests;

TEST_F(data_Test, Unit)
{
	using namespace data;
	ASSERT_EQ(Unit(0u, "?"), Unit::UNKNOWN) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("?", Unit::UNKNOWN._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(0u, Unit::UNKNOWN._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(1u, "B"), Unit::BYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("B", Unit::BYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(1u, Unit::BYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(2u, "kB"), Unit::KILOBYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("kB", Unit::KILOBYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(2u, Unit::KILOBYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(3u, "MB"), Unit::MEGABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("MB", Unit::MEGABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(3u, Unit::MEGABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(4u, "GB"), Unit::GIGABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("GB", Unit::GIGABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(4u, Unit::GIGABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(5u, "TB"), Unit::TERABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("TB", Unit::TERABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(5u, Unit::TERABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(6u, "PB"), Unit::PETABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("PB", Unit::PETABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(6u, Unit::PETABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(7u, "EB"), Unit::EXABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("EB", Unit::EXABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(7u, Unit::EXABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(8u, "ZB"), Unit::ZETTABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("ZB", Unit::ZETTABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(8u, Unit::ZETTABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";

	ASSERT_EQ(Unit(9u, "YB"), Unit::YOTTABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("YB", Unit::YOTTABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(9u, Unit::YOTTABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Size)
{
	using namespace data;
	for (auto& size : sizes) {
		ASSERT_EQ(size.sizes.size(), sz.size()) << "Invalid Test! Vector size mismatch!";
		for (int i{ 0 }; i < size.sizes.size() && i < sz.size(); ++i) {
			ASSERT_EQ(size[i], Size(size.unit, sz[i])) << "Size Constructors are not deterministic!";
			ASSERT_EQ(size[i]._type, size.unit) << "Size Constructors fail to correctly set unit!";
			ASSERT_EQ(size[i]._value, sz[i]) << "Size Constructors fail to correctly set value!";
			ASSERT_EQ(size[i].convert_to(size.unit), size[i]) << "Size conversion fails when converting to the same unit!";
			const auto build_byte{ [](auto&& sz) -> Size {
				const auto diff_exp{ static_cast<long double>(Unit::BYTE.operator unsigned int()) - static_cast<long double>(sz._type.operator unsigned int()) };
				const auto div{ pow(1024.0, diff_exp) };
				return { Unit::BYTE, sz._value / div };
			} };
			ASSERT_EQ(size[i].convert_to(Unit::BYTE), build_byte(size[i])) << "Conversion algorithm was modified resulting in incorrect conversions!";
		}
	}
}
#pragma endregion data


#pragma region base
#include "base_Test.hpp"
using namespace base::unittests;

TEST_F(base_Test, Conversions)
{
	const auto conv{ [](const std::string& arg) {
		bool negative{ arg.front() == '-' };
		if (std::string argstr{ negative ? arg.substr(1u) : arg }; argstr.find("0x") == 0u || std::any_of(argstr.begin(), argstr.end(), [](auto&& c) { const auto ch{ str::toupper(c) };  return ch >= 'A' && ch <= 'F'; })) // hex input
			std::cout << (negative ? "-" : "") << IntPrinter(base::to_decimal(argstr)) << '\n';
		else // decimal input:
			std::cout << (negative ? "-" : "") << (OutputSettings.hide_types ? "" : "0x") << str::hex(str::stoll(argstr), std::uppercase, (OutputSettings.number_grouping ? str::NumberGrouping : str::Placeholder)) << '\n';

	} };
	for (size_t i{ 0ull }; i < decimal_vals.size() && i < hex_vals.size(); ++i) {

	}
}
#pragma endregion base