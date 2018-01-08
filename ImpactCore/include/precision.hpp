#pragma once
#include <cstdint>
#include <limits>

#pragma warning(disable: 4305) // truncation from 'double' to 'float'

namespace Impact {

// Typedefs for precision

// Single precision
typedef float imp_float;
typedef uint32_t imp_float_bits;

// Double precision
//typedef double imp_float;
//typedef uint64_t imp_float_bits;

constexpr imp_float IMP_FLOAT_MAHCINE_EPS = std::numeric_limits<imp_float>::epsilon()/2;

} // Impact
