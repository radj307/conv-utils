#include "pch.h"

#include "data_Test.hpp"
using namespace data::unittests;

#include "base_Test.hpp"
using namespace base::unittests;

#include "modulo_Test.hpp"
using namespace modulo::unittests;


TEST_F(data_Test, Convert_Unknown)
{
	ASSERT_EQ(data::Unit(0u, "?"), data::Unit::UNKNOWN) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("?", data::Unit::UNKNOWN._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(0u, data::Unit::UNKNOWN._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_Byte)
{
	ASSERT_EQ(data::Unit(1u, "B"), data::Unit::BYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("B", data::Unit::BYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(1u, data::Unit::BYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_KiloByte)
{
	ASSERT_EQ(data::Unit(2u, "kB"), data::Unit::KILOBYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("kB", data::Unit::KILOBYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(2u, data::Unit::KILOBYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_MegaByte)
{
	ASSERT_EQ(data::Unit(3u, "MB"), data::Unit::MEGABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("MB", data::Unit::MEGABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(3u, data::Unit::MEGABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_GigaByte)
{
	ASSERT_EQ(data::Unit(4u, "GB"), data::Unit::GIGABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("GB", data::Unit::GIGABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(4u, data::Unit::GIGABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_TeraByte)
{
	ASSERT_EQ(data::Unit(5u, "TB"), data::Unit::TERABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("TB", data::Unit::TERABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(5u, data::Unit::TERABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_PetaByte)
{
	ASSERT_EQ(data::Unit(6u, "PB"), data::Unit::PETABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("PB", data::Unit::PETABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(6u, data::Unit::PETABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_ExaByte)
{
	ASSERT_EQ(data::Unit(7u, "EB"), data::Unit::EXABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("EB", data::Unit::EXABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(7u, data::Unit::EXABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_ZettaByte)
{
	ASSERT_EQ(data::Unit(8u, "ZB"), data::Unit::ZETTABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("ZB", data::Unit::ZETTABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(8u, data::Unit::ZETTABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
}
TEST_F(data_Test, Convert_YottaByte)
{
	ASSERT_EQ(data::Unit(9u, "YB"), data::Unit::YOTTABYTE) << "[!]:\t" << "Unit Constructors are not deterministic!";
	ASSERT_EQ("YB", data::Unit::YOTTABYTE._sym) << "[!]:\t" << "Unit Constructors don't correctly set symbol!";
	ASSERT_EQ(9u, data::Unit::YOTTABYTE._index) << "[!]:\t" << "Unit Constructors don't correctly set index!";
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
			ASSERT_EQ(size[i].convert_to(Unit::BYTE), build_byte(size[i])) << "Conversion algorithm was modified resulting in incorrect results!";
		}
	}
}


TEST_F(base_Test, ToDecimal)
{
	for (size_t i{ 0ull }; i < decimal_vals.size() && i < hex_vals.size(); ++i) {
		ASSERT_EQ(base::to_decimal(hex_vals.at(i)), decimal_vals.at(i)) << "Conversion to decimal failed!";
	}
}
TEST_F(base_Test, ToHex)
{
	for (size_t i{ 0ull }; i < decimal_vals.size() && i < hex_vals.size(); ++i) {
		ASSERT_EQ(base::to_decimal(hex_vals.at(i)), decimal_vals.at(i)) << "Conversion to hex failed!";
	}
}


TEST_F(modulo_Test, Integrals)
{
	ASSERT_EQ(modulo::Calculate<IntT>(17, 3).getResult(), 2) << "Integral modulo failed!";
	ASSERT_EQ(modulo::Calculate<IntT>(256, 5).getResult(), 1) << "Integral modulo failed!";
	ASSERT_EQ(modulo::Calculate<IntT>(1234567890, 321).getResult(), 285) << "Integral modulo failed!";
}
TEST_F(modulo_Test, FloatingPoints)
{
	ASSERT_TRUE((modulo::Calculate<FloatT>(12345.67890, 3.21).getResult() - 0.01890000104410472) < 1e-10) << "Floating-Point modulo failed!";
	ASSERT_TRUE((modulo::Calculate<FloatT>(0.05, 1).getResult() - 0.05) < 1e-10) << "Floating-Point modulo failed!";
	ASSERT_TRUE((modulo::Calculate<FloatT>(78.5, 56.1).getResult() - 22.4) < 1e-10) << "Floating-Point modulo failed!";
}
