#pragma once
#include "pch.h"
#include <base.hpp>
namespace base::unittests {
	class base_Test : public ::testing::Test {
	protected:
		std::vector<int> decimal_vals{
			-199999999,
			-12459,
			-1024,
			-50,
			-5,
			0,
			5,
			50,
			1024,
			12459,
			199999999,
		};
		std::vector<std::string> hex_vals{
			"-0xBEBC1FF",
			"-0x30AB",
			"-0x400",
			"-0x32",
			"-0x5",
			"0x0",
			"0x5",
			"0x32",
			"0x400",
			"0x30AB",
			"0xBEBC1FF",
		};

		//void SetUp() const {}
		//void TearDown() const {}
	};
}