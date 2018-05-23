#include "StratifiedSampler.hpp"
#include "sampling.hpp"
#include "api.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// StratifiedSampler method definitions

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

// StratifiedSampler function definitions

Sampler* createStratifiedSampler(const ParameterSet& parameters)
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
						 "Type:", "Stratified",
						 "Samples per pixel:", horizontal_samples*vertical_samples,
						 "Horizontal samples:", horizontal_samples,
						 "Vertical samples:", vertical_samples,
						 "Sample dimensions:", sample_dimensions);
	}

    return new StratifiedSampler(horizontal_samples, vertical_samples, sample_dimensions);
}

} // RayImpact
} // Impact
