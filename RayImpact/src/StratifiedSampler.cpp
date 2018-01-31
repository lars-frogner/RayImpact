#include "StratifiedSampler.hpp"
#include <algorithm>

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

// Utility functions for stratified sampling

// Fills the given array with stratified sample values covering the unit interval
void generateStratifiedSamples(imp_float* samples,
                               size_t n_samples,
                               RandomNumberGenerator& rng)
{
    imp_float sample_separation = 1.0f/n_samples;

    for (size_t sample_idx = 0; sample_idx < n_samples; sample_idx++)
    {
        samples[sample_idx] = std::min((sample_idx + rng.uniformFloat())*sample_separation, IMP_ONE_MINUS_EPS);
    }
}

// Fills the given array with stratified sample points covering the unit square
void generateStratifiedSamples(Point2F* samples,
                               size_t n_samples_x,
                               size_t n_samples_y,
                               RandomNumberGenerator& rng)
{
    imp_float sample_separation_x = 1.0f/n_samples_x;
    imp_float sample_separation_y = 1.0f/n_samples_y;
    size_t sample_idx = 0;

    for (size_t y = 0; y < n_samples_y; y++) {
        for (size_t x = 0; x < n_samples_x; x++)
        {
            samples[sample_idx].x = std::min((x + rng.uniformFloat())*sample_separation_x, IMP_ONE_MINUS_EPS);
            samples[sample_idx].y = std::min((y + rng.uniformFloat())*sample_separation_y, IMP_ONE_MINUS_EPS);
        
            sample_idx++;
        }
    }
}

// Fills the given array with Latin hypercube sample points inside the n-dimensional unit cube
void generateLatinHypercubeSamples(imp_float* samples,
                                   size_t n_samples,
                                   unsigned int n_sample_dimensions,
                                   RandomNumberGenerator& rng)
{
    imp_float sample_separation = 1.0f/n_samples;

    // Generate stratified sample values in the unit interval for all sample dimensions
    for (size_t sample_idx = 0; sample_idx < n_samples; sample_idx++) {
        for (unsigned int n = 0; n < n_sample_dimensions; n++)
        {
            imp_float sample_value = (sample_idx + rng.uniformFloat())*sample_separation;

            samples[sample_idx*n_sample_dimensions + n] = std::min(sample_value, IMP_ONE_MINUS_EPS);
        }
    }

    // Shuffle the sample values in each dimension
    for (unsigned int n = 0; n < n_sample_dimensions; n++) {
        for (size_t sample_idx = 0; sample_idx < n_samples; sample_idx++)
        {
            // Choose random sample higher up in the list
            size_t sample_to_swap_with = sample_idx + rng.uniformUInt32();
    
            // Swap the samples
            std::swap(samples[         sample_idx*n_sample_dimensions + n],
                      samples[sample_to_swap_with*n_sample_dimensions + n]);
        }
    }
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
