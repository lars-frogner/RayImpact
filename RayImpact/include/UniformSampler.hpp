#pragma once
#include "Sampler.hpp"

namespace Impact {
namespace RayImpact {

// UniformSampler declarations

class UniformSampler : public PixelSampler {

public:

	UniformSampler(unsigned int n_samples_per_pixel,
				   unsigned int n_sampled_dimensions);
	

	void setPixel(const Point2I& pixel);
	
	std::unique_ptr<Sampler> cloned();
	std::unique_ptr<Sampler> cloned(unsigned int seed);
};

} // RayImpact
} // Impact
