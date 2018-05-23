#pragma once
#include "precision.hpp"
#include <string>
#include <cmath>

namespace Impact {

// Image utility function declarations

bool writePFM(const std::string& filename,
              const imp_float* pixel_values,
              unsigned int width, unsigned int height,
			  float pixel_scale);

// Image utility inline function definitions

inline float gammaEncoded(float pixel_value)
{
	return std::sqrt(pixel_value);
}

} // Impact