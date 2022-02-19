/**
 * @file	FOV.hpp
 * @author	radj307
 * @brief	Utilities for converting between Vertical, Horizontal, & Diagonal FOV values in games.
 */
#pragma once
#include "radians.hpp"

#include <utility>

namespace FOV {
	using value = long double;

	struct AspectRatio {
		unsigned h, v;

		constexpr AspectRatio(unsigned const& horizontal, unsigned const& vertical) : h{ horizontal }, v{ vertical } {}
		constexpr AspectRatio(std::pair<unsigned, unsigned> const& pr) : h{ pr.first }, v{ pr.second } {}

		constexpr value verticalOverHorizontal() const
		{
			return static_cast<value>(v) / static_cast<value>(h);
		}
		constexpr value horizontalOverVertical() const
		{
			return static_cast<value>(h) / static_cast<value>(v);
		}

		// @brief	16:9 Aspect Ratio
		static const AspectRatio AR16x9;
		// @brief	16:10 Aspect Ratio
		static const AspectRatio AR16x10;
	};

	constexpr const AspectRatio AspectRatio::AR16x9{ 16u, 9u };
	constexpr const AspectRatio AspectRatio::AR16x10{ 16u, 10u };

	/**
	 * @brief				Convert a horizontal FOV value in degrees to a vertical FOV value in degrees, given an aspect ratio.
	 * @param horizontal	Input Horizontal FOV value in degrees.
	 * @param aspect		Aspect Ratio.
	 * @returns				value
	 */
	[[nodiscard]] inline value toVertical(value const& horizontal, AspectRatio const& aspect)
	{
		return toDegrees( 2 * std::atan(std::tan(toRadians(horizontal) / 2) * aspect.verticalOverHorizontal()) );
	}

	/**
	 * @brief				Convert a vertical FOV value in degrees to a horizontal FOV value in degrees, given an aspect ratio.
	 * @param vertical		Input Vertical FOV value in degrees.
	 * @param aspect		Aspect Ratio.
	 * @returns				value
	 */
	[[nodiscard]] inline value toHorizontal(value const& vertical, AspectRatio const& aspect)
	{
		return toDegrees( 2 * std::atan(std::tan(toRadians(vertical) / 2) * aspect.horizontalOverVertical()) );
	}
}