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

// StratifiedSampler function declarations

Sampler* createStratifiedSampler(const ParameterSet& parameters);

// StratifiedSampler inline method definitions

inline StratifiedSampler::StratifiedSampler(unsigned int n_horizontal_samples_per_pixel,
											unsigned int n_vertical_samples_per_pixel,
											unsigned int n_sampled_dimensions)
    : PixelSampler::PixelSampler(n_horizontal_samples_per_pixel*n_vertical_samples_per_pixel, n_sampled_dimensions),
      n_horizontal_samples_per_pixel(n_horizontal_samples_per_pixel),
      n_vertical_samples_per_pixel(n_vertical_samples_per_pixel)
{}

} // RayImpact
} // Impact
