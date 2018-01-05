#pragma once
#include "precision.hpp"
#include <limits>

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

inline int sign(imp_float val)
{
    return (imp_float(0) < val) - (val < imp_float(0));
}

// Linear interpolation between two values
inline imp_float lerp(imp_float value_1, imp_float value_2, imp_float weight)
{
    return (1.0f - weight)*value_1 + weight*value_2;
}

// Truncates the given value to keep it inside the given range
inline imp_float clamp(imp_float value, imp_float lower_limit, imp_float upper_limit)
{
	if (value < lower_limit)
		value = lower_limit;
	else if (value > upper_limit)
		value = upper_limit;

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

} // Impact