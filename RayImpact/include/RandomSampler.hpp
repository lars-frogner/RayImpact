#pragma once
#include "Sampler.hpp"
#include "ParameterSet.hpp"
#include <memory>

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

// RandomSampler function declarations

Sampler* createRandomSampler(const ParameterSet& parameters);

// RandomSampler inline method definitions

inline RandomSampler::RandomSampler(unsigned int n_samples_per_pixel,
									unsigned int n_sampled_dimensions)
    : PixelSampler::PixelSampler(n_samples_per_pixel, n_sampled_dimensions)
{}

} // RayImpact
} // Impact
