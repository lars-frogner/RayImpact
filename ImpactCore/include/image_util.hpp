#pragma once
#include "precision.hpp"
#include <string>

namespace Impact {

typedef unsigned char imp_ppm_data;

imp_ppm_data* readPPM(const std::string& filename,
					  imp_float*& data,
					  unsigned int& width, unsigned int& height, unsigned int& n_components,
					  unsigned int range_type,
					  bool is_texture = true,
					  imp_ppm_data* input_buffer = nullptr,
					  bool keep_input_buffer = false);

imp_ppm_data* writePPM(const std::string& filename,
					   const imp_float* data,
					   unsigned int width, unsigned int height, unsigned int n_components,
					   unsigned int range_type,
					   imp_ppm_data* output_buffer = nullptr,
					   bool keep_output_buffer = false);

} // Impact