#include "UniformSampler.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// UniformSampler method implementations
    
UniformSampler::UniformSampler(unsigned int n_horizontal_samples_per_pixel,
                               unsigned int n_vertical_samples_per_pixel,
                               unsigned int n_sampled_dimensions)
    : PixelSampler::PixelSampler(n_horizontal_samples_per_pixel*n_vertical_samples_per_pixel, n_sampled_dimensions),
      n_horizontal_samples_per_pixel(n_horizontal_samples_per_pixel),
      n_vertical_samples_per_pixel(n_vertical_samples_per_pixel)
{}
    
void UniformSampler::setPixel(const Point2I& pixel)
{
    PixelSampler::setPixel(pixel);

    imp_float sample_separation = 1.0f/n_samples_per_pixel;
    imp_float sample_separation_x = 1.0f/n_horizontal_samples_per_pixel;
    imp_float sample_separation_y = 1.0f/n_vertical_samples_per_pixel;

    // Generate 1D sample components
    for (size_t k = 0; k < sample_components_1D.size(); k++)
    {
        for (size_t i = 0; i < n_samples_per_pixel; i++)
        {
            sample_components_1D[k][i] = (i + 0.5f)*sample_separation;
        }
    }
    
    // Generate 2D sample components
    for (size_t k = 0; k < sample_components_2D.size(); k++)
    {
        size_t idx = 0;

        for (size_t j = 0; j < n_vertical_samples_per_pixel; j++) {
            for (size_t i = 0; i < n_horizontal_samples_per_pixel; i++)
            {
                sample_components_2D[k][idx].x = (i + 0.5f)*sample_separation_x;
                sample_components_2D[k][idx].y = (j + 0.5f)*sample_separation_y;

                idx++;
            }
        }
    }

    // Generate 1D sample component arrays
    for (size_t k = 0; k < sample_component_arrays_1D.size(); k++)
    {    
        size_t array_size = sizes_of_1D_component_arrays[k];

        size_t idx = 0;

        for (size_t i = 0; i < n_samples_per_pixel; i++)
        {
            imp_float sample = (i + 0.5f)*sample_separation;

            for (size_t n = 0; n < array_size; n++)
            {
                sample_component_arrays_1D[k][idx++] = sample;
            }
        }
    }

    // Generate 2D sample component arrays
    for (size_t k = 0; k < sample_component_arrays_2D.size(); k++)
    {    
        size_t array_size = sizes_of_2D_component_arrays[k];

        size_t idx = 0;
        
        for (size_t j = 0; j < n_vertical_samples_per_pixel; j++) {
            for (size_t i = 0; i < n_horizontal_samples_per_pixel; i++)
            {
                imp_float sample_x = (i + 0.5f)*sample_separation_x;
                imp_float sample_y = (j + 0.5f)*sample_separation_y;

                for (size_t n = 0; n < array_size; n++)
                {
                    sample_component_arrays_2D[k][idx].x = sample_x;
                    sample_component_arrays_2D[k][idx].y = sample_y;
                    
                    idx++;
                }
            }
        }
    }
}

std::unique_ptr<Sampler> UniformSampler::cloned()
{
    UniformSampler* sampler = new UniformSampler(*this);
    
    sampler->rng.setRandomSeed();

    return std::unique_ptr<Sampler>(sampler);
}

std::unique_ptr<Sampler> UniformSampler::cloned(unsigned int seed)
{
    UniformSampler* sampler = new UniformSampler(*this);
    
    sampler->rng.setSeed(seed);

    return std::unique_ptr<Sampler>(sampler);
}

// UniformSampler creation

Sampler* createUniformSampler(const ParameterSet& parameters)
{
    unsigned int n_pix_samples_x = (unsigned int)std::abs(parameters.getSingleIntValue("n_pix_samples_x", 1));
    unsigned int n_pix_samples_y = (unsigned int)std::abs(parameters.getSingleIntValue("n_pix_samples_y", 1));
    unsigned int n_sample_dims = (unsigned int)std::abs(parameters.getSingleIntValue("n_sample_dims", 5));

    return new UniformSampler(n_pix_samples_x, n_pix_samples_y, n_sample_dims);
}

} // RayImpact
} // Impact
