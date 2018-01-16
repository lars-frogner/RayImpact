#include "image_util.hpp"
#include "error.hpp"
#include <fstream>
#include <memory>

namespace Impact {

bool writePFM(const std::string& filename,
			  const imp_float* pixel_values,
			  unsigned int width, unsigned int height)
{
	imp_check(pixel_values);

	float scale = (machineIsBigEndian())? 1.0f : -1.0f;

	// Create buffer for reorganized and converted pixel values
	std::unique_ptr<float[]> output_buffer(new float[3*width*height]);

	unsigned int idx = 0;
	unsigned int pixel_idx;

	// Loop through image row by row from bottom to top and store values in output buffer
	for (int j = (int)height - 1; j >= 0; j--) {	
		for (int i = 0; i < (int)width; i++)
		{
			pixel_idx = 3*(j*width + i);

			output_buffer[idx++] = (float)pixel_values[pixel_idx    ];
			output_buffer[idx++] = (float)pixel_values[pixel_idx + 1];
			output_buffer[idx++] = (float)pixel_values[pixel_idx + 2];
		}
	}
	
	// Open file for output
	std::ofstream file;
    file.open(filename.c_str(), std::ios::binary);

	if (!file.is_open())
	{
		printErrorMessage("cannot open file \"%s\" for output", filename);
		return false;
	}
	
	// Write header
	file << "PF" << std::endl;
	file << width << " " << height << std::endl;
	file << scale << std::endl;

	// Write data
    file.write(reinterpret_cast<char*>(&output_buffer[0]), 3*width*height*sizeof(float));
	file.close();

	return true;
}

} // Impact