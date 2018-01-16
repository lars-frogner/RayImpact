#pragma once
#include <string>

namespace Impact {
namespace RayImpact {

struct Options
{
	unsigned int n_threads = 0; // The number of threads to use for parallelization (determined automatically if set to 0)
	std::string image_filename; // The filename to use for the rendered image
};

extern Options RIMP_OPTIONS; // Global rendering options

// API function declarations

void RIMP_Initialize(const Options& options);

void RIMP_Cleanup();

} // RayImpact
} // Impact
