#pragma once
#include "precision.hpp"
#include "error.hpp"
#include <limits>
#include <utility>
#include <functional>
#include <cmath>

namespace Impact {

// Numerical constants

constexpr imp_float IMP_INFINITY = std::numeric_limits<imp_float>::infinity();
constexpr imp_float IMP_MAX = std::numeric_limits<imp_float>::max();
constexpr imp_float IMP_MIN = std::numeric_limits<imp_float>::min();
constexpr imp_float IMP_LOWEST = std::numeric_limits<imp_float>::lowest();

constexpr imp_float IMP_PI = 3.14159265358979323846;
constexpr imp_float IMP_PI_OVER_TWO = 1.57079632679489661923;
constexpr imp_float IMP_THREE_PI_OVER_TWO = 3*IMP_PI_OVER_TWO;
constexpr imp_float IMP_TWO_PI = 2*IMP_PI;
constexpr imp_float IMP_FOUR_PI = 4*IMP_PI;
constexpr imp_float IMP_DEG_TO_RAD = IMP_PI/180;
constexpr imp_float IMP_RAD_TO_DEG = 180/IMP_PI;

// Utility functions

template <typename T>
inline bool isNaN(const T x)
{
    return std::isnan(x);
}

template <>
inline bool isNaN(const int x)
{
    return false;
}

inline int sign(imp_float val)
{
    return (imp_float(0) < val) - (val < imp_float(0));
}

// Linear interpolation between two values
template <typename T, typename U>
inline T lerp(U value_1, U value_2, T weight)
{
    return (1 - weight)*value_1 + weight*value_2;
}

// Truncates the given value to keep it inside the given range
template <typename T, typename U>
inline T clamp(T value, U lower_limit, U upper_limit)
{
	if (value < lower_limit)
		return lower_limit;

	if (value > upper_limit)
		return upper_limit;

	return value;
}

// Conversion from degrees to radians
inline imp_float degreesToRadians(imp_float angle_deg)
{
	return angle_deg*IMP_DEG_TO_RAD;
}

// Conversion from radians to degrees
inline imp_float radiansToDegrees(imp_float angle_rad)
{
	return angle_rad*IMP_RAD_TO_DEG;
}

// Solves the quadratic equation a*x^2 + b*x + c = 0 for the solutions x1 (smallest) and x2 (largest) if they exist
inline bool solveQuadraticEquation(imp_float a, imp_float b, imp_float c, imp_float* x1, imp_float* x2)
{
	imp_assert(x1 && x2);
	imp_assert(a != 0);

	double discriminant = (double)(b)*(double)(b) - 4.0*(double)(a)*(double)(c);

	if (discriminant < 0)
		return false;

	double sqrt_discriminant = std::sqrt(discriminant);

	double q = (b < 0)? -0.5*(b - sqrt_discriminant) : -0.5*(b + sqrt_discriminant);

	*x1 = (imp_float)(q/a);
	*x2 = (imp_float)(c/q);

	if (*x1 > *x2) std::swap(*x1, *x2);

	return true;
}

// Returns the last index in an array of size n_values where the given condition returns true
inline unsigned int findLastIndexWhere(const std::function<bool (unsigned int)>& condition, unsigned int n_values)
{
	unsigned int first_idx = 0;
	unsigned int length = n_values;

	while (length > 0)
	{
		// Compute midpoint
		unsigned int half_length = length >> 1;
		unsigned int middle_idx = first_idx + half_length;

		if (condition(middle_idx)) // Does the condition still hold at the midpoint?
		{
			// Continue search in the second half
			first_idx = middle_idx + 1;
			length -= half_length + 1;
		}
		else
		{
			// Continue search in the first half
			length = half_length;
		}
	}

	return clamp(first_idx-1, 0u, n_values-2);
}

} // Impact