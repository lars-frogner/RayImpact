#include "Sampler.hpp"

namespace Impact {
namespace RayImpact {

// Sampler method implementations

Sampler::Sampler(unsigned int n_samples_per_pixel)
	: n_samples_per_pixel(n_samples_per_pixel)
{}

void Sampler::setPixel(const Point2I& pixel)
{
	current_pixel = pixel;
	current_pixel_sample_idx = 0;

	current_1D_array_component = 0;
	current_2D_array_component = 0;
}

bool Sampler::beginNextSample()
{
	current_1D_array_component = 0;
	current_2D_array_component = 0;

	current_pixel_sample_idx++;

	return current_pixel_sample_idx < n_samples_per_pixel;
}

bool Sampler::beginSampleIndex(size_t pixel_sample_idx)
{
	current_1D_array_component = 0;
	current_2D_array_component = 0;

	current_pixel_sample_idx = pixel_sample_idx;

	return current_pixel_sample_idx < n_samples_per_pixel;
}

CameraSample Sampler::generateCameraSample(const Point2I& pixel)
{
	CameraSample sample;

	sample.sensor_point = static_cast<Point2F>(pixel) + next2DSampleComponent();
	sample.time = next1DSampleComponent();
	sample.lens_point = next2DSampleComponent();

	return sample;
}

unsigned int Sampler::roundedArraySize(unsigned int n_samples) const
{
	return n_samples;
}

void Sampler::createArraysForNext1DSampleComponent(unsigned int n_values)
{
	sizes_of_1D_component_arrays.push_back(n_values);
	sample_component_arrays_1D.push_back(std::vector<imp_float>(n_values*n_samples_per_pixel));
}
	
void Sampler::createArraysForNext2DSampleComponent(unsigned int n_values)
{
	sizes_of_2D_component_arrays.push_back(n_values);
	sample_component_arrays_2D.push_back(std::vector<Point2F>(n_values*n_samples_per_pixel));
}

const imp_float* Sampler::arrayOfNext1DSampleComponent(unsigned int n_values)
{
	if (current_1D_array_component == sample_component_arrays_1D.size())
	{
		return nullptr;
	}

	return &(sample_component_arrays_1D[current_1D_array_component++][current_pixel_sample_idx*n_values]);
}

const Point2F* Sampler::arrayOfNext2DSampleComponent(unsigned int n_values)
{
	if (current_2D_array_component == sample_component_arrays_2D.size())
	{
		return nullptr;
	}

	return &(sample_component_arrays_2D[current_2D_array_component++][current_pixel_sample_idx*n_values]);
}

// PixelSampler method implementations

PixelSampler::PixelSampler(unsigned int n_samples_per_pixel,
						   unsigned int n_sampled_dimensions)
	: Sampler::Sampler(n_samples_per_pixel),
	  current_1D_component(0),
	  current_2D_component(0),
	  rng()
{
	for (unsigned int i = 0; i < n_sampled_dimensions; i++)
	{
		sample_components_1D.push_back(std::vector<imp_float>(n_samples_per_pixel));
		sample_components_2D.push_back(std::vector<Point2F>(n_samples_per_pixel));
	}
}

void PixelSampler::setPixel(const Point2I& pixel)
{
	Sampler::setPixel(pixel);

	current_1D_component = 0;
	current_2D_component = 0;
}

bool PixelSampler::beginNextSample()
{
	current_1D_component = 0;
	current_2D_component = 0;

	return Sampler::beginNextSample();
}

bool PixelSampler::beginSampleIndex(size_t pixel_sample_idx)
{
	current_1D_component = 0;
	current_2D_component = 0;

	return Sampler::beginSampleIndex(pixel_sample_idx);
}

imp_float PixelSampler::next1DSampleComponent()
{
	if (current_1D_component < sample_components_1D.size())
	{
		return sample_components_1D[current_1D_component++][current_pixel_sample_idx];
	}
	else
	{
		return rng.uniformFloat();
	}
}

Point2F PixelSampler::next2DSampleComponent()
{
	if (current_2D_component < sample_components_2D.size())
	{
		return sample_components_2D[current_2D_component++][current_pixel_sample_idx];
	}
	else
	{
		return Point2F(rng.uniformFloat(), rng.uniformFloat());
	}
}

// GlobalSampler method implementations

GlobalSampler::GlobalSampler(unsigned int n_samples_per_pixel)
	: Sampler::Sampler(n_samples_per_pixel)
{}

void GlobalSampler::setPixel(const Point2I& pixel)
{
	Sampler::setPixel(pixel);

	next_sample_dimension = 0;
	current_global_sample_idx = pixelToGlobalSampleIndex(0);

	array_end_dimension = array_start_dimension + (unsigned int)sample_component_arrays_1D.size() +
												2*(unsigned int)sample_component_arrays_2D.size();

	// Fill 1D sample component arrays

	for (unsigned int array_dimension = 0; array_dimension < (unsigned int)sample_component_arrays_1D.size(); array_dimension++)
	{
		size_t n_pixel_samples = sizes_of_1D_component_arrays[array_dimension]*n_samples_per_pixel;

		for (size_t pixel_sample_idx = 0; pixel_sample_idx < n_pixel_samples; pixel_sample_idx++)
		{
			size_t global_sample_idx = pixelToGlobalSampleIndex(pixel_sample_idx);

			sample_component_arrays_1D[array_dimension][pixel_sample_idx] = 
				valueOfGlobalSampleDimension(global_sample_idx, array_start_dimension + array_dimension);
		}
	}

	// Fill 2D sample component arrays

	unsigned int dimension = array_start_dimension + (unsigned int)sample_component_arrays_1D.size();

	for (unsigned int array_dimension = 0; array_dimension < (unsigned int)sample_component_arrays_2D.size(); array_dimension++)
	{
		size_t n_pixel_samples = sizes_of_2D_component_arrays[array_dimension]*n_samples_per_pixel;

		for (size_t pixel_sample_idx = 0; pixel_sample_idx < n_pixel_samples; pixel_sample_idx++)
		{
			size_t global_sample_idx = pixelToGlobalSampleIndex(pixel_sample_idx);

			sample_component_arrays_2D[array_dimension][pixel_sample_idx].x = valueOfGlobalSampleDimension(global_sample_idx, dimension);
			sample_component_arrays_2D[array_dimension][pixel_sample_idx].y = valueOfGlobalSampleDimension(global_sample_idx, dimension + 1);
		}

		dimension += 2;
	}
}

bool GlobalSampler::beginNextSample()
{
	next_sample_dimension = 0;
	current_global_sample_idx = pixelToGlobalSampleIndex(current_pixel_sample_idx + 1);

	return Sampler::beginNextSample();
}

bool GlobalSampler::beginSampleIndex(size_t pixel_sample_idx)
{
	next_sample_dimension = 0;
	current_global_sample_idx = pixelToGlobalSampleIndex(pixel_sample_idx);

	return Sampler::beginSampleIndex(pixel_sample_idx);
}

imp_float GlobalSampler::next1DSampleComponent()
{
	if (next_sample_dimension >= array_start_dimension && next_sample_dimension < array_end_dimension)
	{
		next_sample_dimension = array_end_dimension;
	}

	return valueOfGlobalSampleDimension(current_global_sample_idx, next_sample_dimension++);
}

Point2F GlobalSampler::next2DSampleComponent()
{
	if (next_sample_dimension + 1 >= array_start_dimension && next_sample_dimension < array_end_dimension)
	{
		next_sample_dimension = array_end_dimension;
	}

	Point2F sample_component(valueOfGlobalSampleDimension(current_global_sample_idx, next_sample_dimension),
							 valueOfGlobalSampleDimension(current_global_sample_idx, next_sample_dimension + 1));

	next_sample_dimension += 2;

	return sample_component;
}

// Sampling utility functions



} // RayImpact
} // Impact
