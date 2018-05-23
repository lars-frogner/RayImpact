#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "RandomNumberGenerator.hpp"
#include "geometry.hpp"
#include <vector>
#include <algorithm>
#include <cmath>

namespace Impact {
namespace RayImpact {

// DistributionFunction1D declarations

class DistributionFunction1D {

public:

    std::vector<imp_float> values; // Values of the distribution function
    std::vector<imp_float> cdf_values; // Values of the corresponding cumulative distribution function
    imp_float integral; // Integral of the distribution function

    DistributionFunction1D(const imp_float* values, unsigned int n_values);

    unsigned int size() const;

    imp_float continuousSample(imp_float uniform_sample,
                               imp_float* pdf_value,
                               unsigned int* offset = nullptr) const;

    unsigned int discreteSample(imp_float uniform_sample,
                                imp_float* pdf_value = nullptr,
                                imp_float* shift = nullptr) const;

    imp_float discretePDF(unsigned int idx) const;
};

// Sampling function declarations

// Fills the given array with stratified sample values covering the unit interval
void generateStratifiedSamples(imp_float* samples,
                               size_t n_samples,
                               RandomNumberGenerator& rng);

// Fills the given array with stratified sample points covering the unit square
void generateStratifiedSamples(Point2F* samples,
                               size_t n_horizontal_samples,
                               size_t n_vertical_samples,
                               RandomNumberGenerator& rng);

// Fills the given array with Latin hypercube sample points inside the n-dimensional unit cube
void generateLatinHypercubeSamples(imp_float* samples,
                                   size_t n_samples,
                                   unsigned int n_sample_dimensions,
                                   RandomNumberGenerator& rng);

Point2F rejectionDiskSample(RandomNumberGenerator& rng);

// Given a sample point inside the unit square, returns a uniformly sampled point inside the unit disk
Point2F uniformDiskSample(const Point2F& uniform_sample);

// Given a sample point inside the unit square, returns a concentric sampled point inside the unit disk
Point2F concentricDiskSample(const Point2F& uniform_sample);

// Given a sample point inside the unit square, returns a uniformly sampled direction vector in the unit hemisphere around the z-axis
Vector3F uniformHemisphereSample(const Point2F& uniform_sample);

// Given a sample point inside the unit square, returns a cosine-weighted sampled direction vector in the unit hemisphere around the z-axis
Vector3F cosineWeightedHemisphereSample(const Point2F& uniform_sample);

// DistributionFunction1D inline method definitions

inline unsigned int DistributionFunction1D::size() const
{
    return (unsigned int)(values.size());
}

inline imp_float DistributionFunction1D::discretePDF(unsigned int idx) const
{
    return values[idx]/(integral*size());
}

// Sampling inline function definitions

// Shuffles the elements in the given array into a random order
template <typename T>
inline void shuffleArray(T* elements,
                         size_t n_elements,
                         RandomNumberGenerator& rng)
{
    for (size_t element = 0; element < n_elements; element++)
    {
        // Choose random element higher up in the list
        size_t element_to_swap_with = element + rng.uniformUInt32((uint32_t)(n_elements - element));

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
            std::swap(elements[                element*n_element_dimensions + n],
                      elements[element_to_swap_with*n_element_dimensions + n]);
        }
    }
}

} // RayImpact
} // Impact
