#pragma once
#include <cstdint>
#include <cstring>
#include <limits>

#pragma warning(disable: 4305) // truncation from 'double' to 'float'

namespace Impact {

// Precision typedefs and constants

#ifndef IMP_FLOAT_IS_DOUBLE
// Single precision
typedef float imp_float;
typedef uint32_t imp_float_bits;
static constexpr imp_float IMP_ONE_MINUS_EPS = 0.99999994f;
#else
// Double precision
typedef double imp_float;
typedef uint64_t imp_float_bits;
static constexpr imp_float IMP_ONE_MINUS_EPS = 0.99999999999999989;
#endif

constexpr imp_float IMP_FLOAT_MAHCINE_EPS = std::numeric_limits<imp_float>::epsilon()/2;

// 1 + errorPowerBound(n) is a conservative bound on (1 +- machine_eps)^n
inline constexpr imp_float errorPowerBound(int n)
{
    return (n*IMP_FLOAT_MAHCINE_EPS)/(1 - n*IMP_FLOAT_MAHCINE_EPS);
}

// Precision function declarations

int machineIsBigEndian();

imp_float closestHigherFloat(imp_float value);

imp_float closestLowerFloat(imp_float value);

// Precision inline function definitions

// Returns the 32/64-bit unsigned integer representing the bit pattern of the given float/double
inline imp_float_bits floatToBits(imp_float value)
{
    imp_float_bits bits;
    std::memcpy(&bits, &value, sizeof(imp_float));
    return bits;
}

// Returns the float/double represented by the bit pattern of the given 32/64-bit unsigned integer
inline imp_float bitsToFloat(imp_float_bits bits)
{
    imp_float value;
    std::memcpy(&value, &bits, sizeof(imp_float_bits));
    return value;
}

} // Impact
