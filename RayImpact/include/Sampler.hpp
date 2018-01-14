#pragma once
#include "precision.hpp"
#include "RandomNumberGenerator.hpp"
#include "geometry.hpp"
#include "Camera.hpp"
#include <vector>
#include <memory>

namespace Impact {
namespace RayImpact {

// Sampler declarations

class Sampler {

private:

	size_t current_1D_array_component; // Index of the current 1D component array
	size_t current_2D_array_component; // Index of the current 2D component array

protected:

	Point2I current_pixel; // Current pixel being sampled
	size_t current_pixel_sample_idx; // Index of the sample currently being generated for the pixel

	std::vector<size_t> sizes_of_1D_component_arrays; // Number of values per sample in each 1D component array
	std::vector<size_t> sizes_of_2D_component_arrays; // Number of values per sample in each 2D component array
	std::vector< std::vector<imp_float> > sample_component_arrays_1D; // 1D sample component arrays
	std::vector< std::vector<Point2F> > sample_component_arrays_2D; // 1D sample component arrays

public:

	const unsigned int n_samples_per_pixel; // Total number of samples that will be generated for each pixel

	Sampler(unsigned int n_samples_per_pixel);

	virtual void setPixel(const Point2I& pixel);

	virtual bool beginNextSample();

	virtual bool beginSampleIndex(size_t pixel_sample_idx);

	CameraSample generateCameraSample(const Point2I& pixel);

	virtual imp_float next1DSampleComponent() = 0;

	virtual Point2F next2DSampleComponent() = 0;

	virtual unsigned int roundedArraySize(unsigned int n_values) const;

	void createArraysForNext1DSampleComponent(unsigned int n_values);
	
	void createArraysForNext2DSampleComponent(unsigned int n_values);

	const imp_float* arrayOfNext1DSampleComponent(unsigned int n_values);

	const Point2F* arrayOfNext2DSampleComponent(unsigned int n_values);

	virtual std::unique_ptr<Sampler> cloned() = 0;
	virtual std::unique_ptr<Sampler> cloned(unsigned int seed) = 0;
};

// PixelSampler declarations

class PixelSampler : public Sampler {

protected:

	unsigned int current_1D_component; // Current 1D sample component to provide
	unsigned int current_2D_component; // Current 2D sample component to provide

	std::vector< std::vector<imp_float> > sample_components_1D; // 1D sample components
	std::vector< std::vector<Point2F> > sample_components_2D; // 2D sample components

	RandomNumberGenerator rng; // Random number generator for sampling

public:

	PixelSampler(unsigned int n_samples_per_pixel,
				 unsigned int n_sampled_dimensions);

	bool beginNextSample();

	bool beginSampleIndex(size_t pixel_sample_idx);

	imp_float next1DSampleComponent();

	Point2F next2DSampleComponent();
};

// GlobalSampler declarations

class GlobalSampler : public Sampler {

private:

	unsigned int next_sample_dimension; // Next dimension of the sample to provide value for
	size_t current_global_sample_idx; // Total number of samples taken across all pixels

	static const unsigned int array_start_dimension = 5; // First component to put into sample component arrays
	unsigned int array_end_dimension; // Last component to put into sample component arrays

public:

	GlobalSampler(unsigned int n_samples_per_pixel);

	void setPixel(const Point2I& pixel);

	bool beginNextSample();

	bool beginSampleIndex(size_t pixel_sample_idx);

	imp_float next1DSampleComponent();

	Point2F next2DSampleComponent();

	virtual size_t pixelToGlobalSampleIndex(size_t pixel_sample_index) = 0;

	virtual imp_float valueOfGlobalSampleDimension(size_t global_sample_index, unsigned int dimension) = 0;
};

// Sampling utility functions

// Shuffles the elements in the given array into a random order
template <typename T>
inline void shuffleArray(T* elements,
						 size_t n_elements,
						 RandomNumberGenerator& rng)
{
	for (size_t element = 0; element < n_elements; element++)
	{
		// Choose random element higher up in the list
		size_t element_to_swap_with = element + rng.uniformUInt32(n_elements - element);
	
		// Swap the elements
		std::swap(elements[element], elements[element_to_swap_with]);
	}
}

// Shuffles the elements (of a given size) in the given array into a random order
template <typename T>
inline void shuffleArray(T* elements,
						 size_t n_elements,
						 unsigned int n_element_dimensions,
						 RandomNumberGenerator& rng)
{
	for (size_t element = 0; element < n_elements; element++)
	{
		// Choose random element higher up in the list
		size_t element_to_swap_with = element + rng.uniformUInt32(n_elements - element);
	
		// Swap the elements
		for (unsigned int n = 0; n < n_element_dimensions; n++)
		{
			std::swap(elements[				element*n_element_dimensions + n],
					  elements[element_to_swap_with*n_element_dimensions + n]);
		}
	}
}

} // RayImpact
} // Impact
