#include "UniformSampler.hpp"
#include "api.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// UniformSampler method definitions

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

// UniformSampler function definitions

Sampler* createUniformSampler(const ParameterSet& parameters)
{
    unsigned int horizontal_samples = (unsigned int)std::abs(parameters.getSingleIntValue("horizontal_samples", 1));
    unsigned int vertical_samples = (unsigned int)std::abs(parameters.getSingleIntValue("vertical_samples", 1));
    unsigned int sample_dimensions = (unsigned int)std::abs(parameters.getSingleIntValue("sample_dimensions", 5));
	
	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Sampler:"
						 "\n    %-20s%s"
						 "\n    %-20s%u"
						 "\n    %-20s%u"
						 "\n    %-20s%u"
						 "\n    %-20s%u",
						 "Type:", "Uniform",
						 "Samples per pixel:", horizontal_samples*vertical_samples,
						 "Horizontal samples:", horizontal_samples,
						 "Vertical samples:", vertical_samples,
						 "Sample dimensions:", sample_dimensions);
	}

    return new UniformSampler(horizontal_samples, vertical_samples, sample_dimensions);
}

} // RayImpact
} // Impact
