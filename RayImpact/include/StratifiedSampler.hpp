#pragma once
#include "Sampler.hpp"

namespace Impact {
namespace RayImpact {

// StratifiedSampler declarations

class StratifiedSampler : public PixelSampler {

private:

	const unsigned int n_horizontal_samples_per_pixel; // Number of samples to take horizontally for each pixel
	const unsigned int n_vertical_samples_per_pixel; // Number of samples to take vertically for each pixel

public:

	StratifiedSampler(unsigned int n_horizontal_samples_per_pixel,
					  unsigned int n_vertical_samples_per_pixel,
					  unsigned int n_sampled_dimensions);
	

	void setPixel(const Point2I& pixel);
	
	std::unique_ptr<Sampler> cloned();
	std::unique_ptr<Sampler> cloned(unsigned int seed);
};

// Utility functions for stratified sampling

void generateStratifiedSamples(imp_float* samples,
						  	   size_t n_samples,
							   RandomNumberGenerator& rng);

void generateStratifiedSamples(Point2F* samples,
							   size_t n_horizontal_samples,
							   size_t n_vertical_samples,
							   RandomNumberGenerator& rng);

void generateLatinHypercubeSamples(imp_float* samples,
								   size_t n_samples,
								   unsigned int n_sample_dimensions,
								   RandomNumberGenerator& rng);

} // RayImpact
} // Impact
