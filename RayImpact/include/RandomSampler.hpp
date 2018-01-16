#pragma once
#include "Sampler.hpp"

namespace Impact {
namespace RayImpact {

// RandomSampler declarations

class RandomSampler : public PixelSampler {

public:

	RandomSampler(unsigned int n_samples_per_pixel,
				   unsigned int n_sampled_dimensions);
	

	void setPixel(const Point2I& pixel);
	
	std::unique_ptr<Sampler> cloned();
	std::unique_ptr<Sampler> cloned(unsigned int seed);
};

} // RayImpact
} // Impact
