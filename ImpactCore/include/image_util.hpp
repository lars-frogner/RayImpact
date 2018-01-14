#pragma once
#include "precision.hpp"
#include <string>

namespace Impact {
	
bool writePFM(const std::string& filename,
			  const imp_float* pixel_values,
			  unsigned int width, unsigned int height);

} // Impact