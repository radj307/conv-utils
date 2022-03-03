/**
 * @file	radians.hpp
 * @author	radj307
 * @brief	Utility functions for converting between degrees & radians.
 */
#pragma once
#include <math.hpp>
#include <var.hpp>

/**
 * @brief			Convert from degrees to radians.
 * @tparam T		Numeric Value Type.
 * @param degrees	Input value in degrees.
 * @returns			T
 */
template<var::numeric T>
static T toRadians(T const& degrees)
{
	return degrees * (M_PI / 180);
}

/**
 * @brief			Convert from radians to degrees.
 * @tparam T		Numeric Value Type. 
 * @param radians	Input value in radians.
 * @returns			T
 */
template<var::numeric T>
static T toDegrees(T const& radians)
{
	return radians * (180 / M_PI);
}

