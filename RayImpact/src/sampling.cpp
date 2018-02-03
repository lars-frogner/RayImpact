#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// Sampling utility function implementations
    
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

// Given a sample point inside the unit square, returns a uniformly sampled point inside the unit disk
Point2F uniformDiskSample(const Point2F& sample_point)
{
    imp_float radius = std::sqrt(sample_point.x);
    imp_float theta = sample_point.y*IMP_TWO_PI;

    return Point2F(radius*std::cos(theta), radius*std::sin(theta));
}

// Given a sample point inside the unit square, returns a concentric sampled point inside the unit disk
Point2F concentricDiskSample(const Point2F& sample_point)
{
    if (sample_point.x == 0 && sample_point.y == 0)
        return Point2F(0, 0);

    imp_float a = 2*sample_point.x - 1;
    imp_float b = 2*sample_point.y - 1;

    imp_float radius;
    imp_float phi;

    if (a*a > b*b)
    {
        // Sample is in the top half of the disk
        radius = a;
        phi = IMP_PI_OVER_FOUR*b/a;
    }
    else
    {
        // Sample is in the bottom half of the disk
        radius = b;
        phi = IMP_PI_OVER_TWO - IMP_PI_OVER_FOUR*a/b;
    }

    return Point2F(radius*std::cos(phi), radius*std::sin(phi));
}

// Given a sample point inside the unit square, returns a uniformly sampled point inside the unit disk
Point2F uniformDiskSample(const Point2F& sample_point)
{
    imp_float radius = std::sqrt(sample_point.x);
    imp_float theta = sample_point.y*IMP_TWO_PI;

    return Point2F(radius*std::cos(theta), radius*std::sin(theta));
}

// Given a sample point inside the unit square, returns a uniformly sampled direction vector in the unit hemisphere around the z-axis
Vector3F uniformHemisphereSample(const Point2F& sample_point)
{
    imp_float cos_theta = sample_point.x;
    imp_float sin_theta = std::sqrt(std::max<imp_float>(0, 1 - cos_theta*cos_theta));
    imp_float phi = IMP_TWO_PI*sample_point.y;

    return Vector3F(sin_theta*std::cos(phi), sin_theta*std::sin(phi), cos_theta);
}

// Given a sample point inside the unit square, returns a cosine-weighted sampled direction vector in the unit hemisphere around the z-axis
Vector3F cosineWeightedHemisphereSample(const Point2F& sample_point)
{
    const Point2F& disk_sample = concentricDiskSample(sample_point);

    imp_float z = std::sqrt(std::max<imp_float>(0, 1 - disk_sample.x*disk_sample.x - disk_sample.y*disk_sample.y));

    return Vector3F(disk_sample.x, disk_sample.y, z);
}

} // RayImpact
} // Impact
