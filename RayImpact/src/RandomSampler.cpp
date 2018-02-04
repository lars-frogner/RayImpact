#include "RandomSampler.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// RandomSampler method implementations

RandomSampler::RandomSampler(unsigned int n_samples_per_pixel,
                             unsigned int n_sampled_dimensions)
    : PixelSampler::PixelSampler(n_samples_per_pixel, n_sampled_dimensions)
{}

void RandomSampler::setPixel(const Point2I& pixel)
{
    PixelSampler::setPixel(pixel);

    // Generate 1D sample components
    for (size_t i = 0; i < sample_components_1D.size(); i++) {
        for (size_t j = 0; j < n_samples_per_pixel; j++)
        {
            sample_components_1D[i][j] = rng.uniformFloat();
        }
    }

    // Generate 2D sample components
    for (size_t i = 0; i < sample_components_2D.size(); i++) {
        for (size_t j = 0; j < n_samples_per_pixel; j++)
        {
            sample_components_2D[i][j].x = rng.uniformFloat();
            sample_components_2D[i][j].y = rng.uniformFloat();
        }
    }

    // Generate 1D sample component arrays
    for (size_t i = 0; i < sample_component_arrays_1D.size(); i++)
    {
        size_t array_size = sizes_of_1D_component_arrays[i];

        for (size_t j = 0; j < n_samples_per_pixel*array_size; j++)
        {
            sample_component_arrays_1D[i][j] = rng.uniformFloat();
        }
    }

    // Generate 2D sample component arrays
    for (size_t i = 0; i < sample_component_arrays_2D.size(); i++)
    {
        size_t array_size = sizes_of_2D_component_arrays[i];

        for (size_t j = 0; j < n_samples_per_pixel*array_size; j++)
        {
            sample_component_arrays_2D[i][j].x = rng.uniformFloat();
            sample_component_arrays_2D[i][j].y = rng.uniformFloat();
        }
    }
}

std::unique_ptr<Sampler> RandomSampler::cloned()
{
    RandomSampler* sampler = new RandomSampler(*this);

    sampler->rng.setRandomSeed();

    return std::unique_ptr<Sampler>(sampler);
}

std::unique_ptr<Sampler> RandomSampler::cloned(unsigned int seed)
{
    RandomSampler* sampler = new RandomSampler(*this);

    sampler->rng.setSeed(seed);

    return std::unique_ptr<Sampler>(sampler);
}

// RandomSampler creation

Sampler* createRandomSampler(const ParameterSet& parameters)
{
    unsigned int n_pix_samples = (unsigned int)std::abs(parameters.getSingleIntValue("n_pix_samples", 1));
    unsigned int n_sample_dims = (unsigned int)std::abs(parameters.getSingleIntValue("n_sample_dims", 5));

    return new RandomSampler(n_pix_samples, n_sample_dims);
}

} // RayImpact
} // Impact
