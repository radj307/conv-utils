#pragma once
#include "pch.h"
namespace data::unittests {
	namespace _internal {
		inline std::vector<Size> construct_size_vec(const Unit& u, const std::vector<double>& szs)
		{
			std::vector<Size> sizes;
			sizes.reserve(szs.size());
			for (auto& sz : szs)
				sizes.emplace_back(Size{ u, sz });
			sizes.shrink_to_fit();
			return sizes;
		}

		struct SizeVecHelper {
			const Unit unit;
			const std::vector<Size> sizes;

			SizeVecHelper(const Unit& u, const std::vector<double>& sz) : unit{ u }, sizes{ construct_size_vec(unit, sz) } {}

			constexpr auto operator[](const unsigned& pos) const { return sizes.at(pos); }

			std::vector<Size> convert_all_to(const Unit& target_type) const
			{
				std::vector<Size> vec;
				vec.reserve(4u);
				for (auto& in : sizes)
					vec.emplace_back(in.convert_to(target_type));
				vec.shrink_to_fit();
				return vec;
			}
		};

	}
	class data_Test : public ::testing::Test {
	protected:
		std::vector<Unit> ValidUnits{
			Unit::BYTE,
			Unit::KILOBYTE,
			Unit::MEGABYTE,
			Unit::GIGABYTE,
			Unit::TERABYTE,
			Unit::PETABYTE,
			Unit::EXABYTE,
			Unit::ZETTABYTE,
			Unit::YOTTABYTE,
		};
		std::vector<double> sz{ 102400.0, 1024.0, 256.0, 0.0 };
		std::vector<_internal::SizeVecHelper> sizes{
			{ Unit::BYTE, sz },
			{ Unit::KILOBYTE, sz },
			{ Unit::MEGABYTE, sz },
			{ Unit::GIGABYTE, sz },
			{ Unit::TERABYTE, sz },
			{ Unit::PETABYTE, sz },
			{ Unit::EXABYTE, sz },
			{ Unit::ZETTABYTE, sz },
			{ Unit::YOTTABYTE, sz },
		};

		//void SetUp() const {}
		//void TearDown() const {}
	};
}