#pragma once
#include "Sampler.hpp"

namespace Impact {
namespace RayImpact {

// StratifiedSampler declarations

class StratifiedSampler : public PixelSampler {

private:

	const unsigned int n_horizontal_samples_per_pixel;
	const unsigned int n_vertical_samples_per_pixel;
	const bool jitter_samples;

public:

	StratifiedSampler(unsigned int n_horizontal_samples_per_pixel,
					  unsigned int n_vertical_samples_per_pixel,
					  bool jitter_samples,
					  unsigned int n_sampled_dimensions);
	

	void setPixel(const Point2I& pixel);
	
	std::unique_ptr<Sampler> cloned();
	std::unique_ptr<Sampler> cloned(unsigned int seed);
};

// Utility functions for stratified sampling

void generateStratifiedSamples(imp_float* samples,
						  	   unsigned int n_samples,
							   RandomNumberGenerator& rng,
							   bool jitter_samples);

void generateStratifiedSamples(Point2F* samples,
							   unsigned int n_horizontal_samples,
							   unsigned int n_vertical_samples,
							   RandomNumberGenerator& rng,
							   bool jitter_samples);

void generateLatinHypercubeSamples(imp_float* samples,
								   unsigned int n_samples,
								   unsigned int n_sample_dimensions,
								   RandomNumberGenerator& rng);

} // RayImpact
} // Impact
