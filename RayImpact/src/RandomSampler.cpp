#include "RandomSampler.hpp"
#include "api.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// RandomSampler method definitions

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

// RandomSampler function definitions

Sampler* createRandomSampler(const ParameterSet& parameters)
{
    unsigned int samples = (unsigned int)std::abs(parameters.getSingleIntValue("samples", 1));
    unsigned int sample_dimensions = (unsigned int)std::abs(parameters.getSingleIntValue("sample_dimensions", 5));
	
	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Sampler:"
						 "\n    %-20s%s"
						 "\n    %-20s%u"
						 "\n    %-20s%u",
						 "Type:", "Random",
						 "Samples per pixel:", samples,
						 "Sample dimensions:", sample_dimensions);
	}

    return new RandomSampler(samples, sample_dimensions);
}

} // RayImpact
} // Impact
