#include "StratifiedSampler.hpp"
#include "sampling.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// StratifiedSampler method implementations

StratifiedSampler::StratifiedSampler(unsigned int n_horizontal_samples_per_pixel,
                                     unsigned int n_vertical_samples_per_pixel,
                                     unsigned int n_sampled_dimensions)
    : PixelSampler::PixelSampler(n_horizontal_samples_per_pixel*n_vertical_samples_per_pixel, n_sampled_dimensions),
      n_horizontal_samples_per_pixel(n_horizontal_samples_per_pixel),
      n_vertical_samples_per_pixel(n_vertical_samples_per_pixel)
{}

void StratifiedSampler::setPixel(const Point2I& pixel)
{
    PixelSampler::setPixel(pixel);

    // Generate 1D sample components
    for (size_t i = 0; i < sample_components_1D.size(); i++)
    {
        generateStratifiedSamples(sample_components_1D[i].data(),
                                  n_samples_per_pixel,
                                  rng);

        shuffleArray(sample_components_1D[i].data(),
                     n_samples_per_pixel,
                     rng);
    }

    // Generate 2D sample components
    for (size_t i = 0; i < sample_components_2D.size(); i++)
    {
        generateStratifiedSamples(sample_components_2D[i].data(),
                                  n_horizontal_samples_per_pixel,
                                  n_vertical_samples_per_pixel,
                                  rng);

        shuffleArray(sample_components_2D[i].data(),
                     n_samples_per_pixel,
                     rng);
    }

    // Generate 1D sample component arrays
    for (size_t i = 0; i < sample_component_arrays_1D.size(); i++) {
        for (size_t j = 0; j < n_samples_per_pixel; j++)
        {
            size_t array_size = sizes_of_1D_component_arrays[i];

            generateStratifiedSamples(&(sample_component_arrays_1D[i][j*array_size]),
                                      array_size, rng);

            shuffleArray(&(sample_component_arrays_1D[i][j*array_size]),
                         array_size, rng);
        }
    }

    // Generate 2D sample component arrays
    for (size_t i = 0; i < sample_component_arrays_2D.size(); i++) {
        for (size_t j = 0; j < n_samples_per_pixel; j++)
        {
            size_t array_size = sizes_of_2D_component_arrays[i];

            // Note: This relies on the x and y components of Point2 objects being contiguous in memory
            generateLatinHypercubeSamples(&(sample_component_arrays_2D[i][j*array_size].x),
                                          array_size, 2, rng);
        }
    }
}

std::unique_ptr<Sampler> StratifiedSampler::cloned()
{
    StratifiedSampler* sampler = new StratifiedSampler(*this);

    sampler->rng.setRandomSeed();

    return std::unique_ptr<Sampler>(sampler);
}

std::unique_ptr<Sampler> StratifiedSampler::cloned(unsigned int seed)
{
    StratifiedSampler* sampler = new StratifiedSampler(*this);

    sampler->rng.setSeed(seed);

    return std::unique_ptr<Sampler>(sampler);
}

// StratifiedSampler creation

Sampler* createStratifiedSampler(const ParameterSet& parameters)
{
    unsigned int n_pix_samples_x = (unsigned int)std::abs(parameters.getSingleIntValue("n_pix_samples_x", 1));
    unsigned int n_pix_samples_y = (unsigned int)std::abs(parameters.getSingleIntValue("n_pix_samples_y", 1));
    unsigned int n_sample_dims = (unsigned int)std::abs(parameters.getSingleIntValue("n_sample_dims", 5));

    return new StratifiedSampler(n_pix_samples_x, n_pix_samples_y, n_sample_dims);
}

} // RayImpact
} // Impact
