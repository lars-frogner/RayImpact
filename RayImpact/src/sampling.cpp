#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// DistributionFunction1D method definitions

DistributionFunction1D::DistributionFunction1D(const imp_float* values, unsigned int n_values)
    : values(values, values + n_values),
      cdf_values(n_values + 1)
{
    imp_float inv_n_values = 1.0f/n_values;

    cdf_values[0] = 0.0f;
    for (unsigned int i = 1; i <= n_values; i++)
        cdf_values[i] = cdf_values[i-1] + values[i-1]*inv_n_values;

    integral = cdf_values[n_values];

    if (integral == 0)
    {
        // Use the CDF of the constant function
        for (unsigned int i = 1; i <= n_values; i++)
            cdf_values[i] = i*inv_n_values;
    }
    else
    {
        imp_float inv_integral = 1.0f/integral;

        for (unsigned int i = 1; i <= n_values; i++)
            cdf_values[i] *= inv_integral;
    }
}

imp_float DistributionFunction1D::continuousSample(imp_float uniform_sample,
                                                   imp_float* pdf_value,
                                                   unsigned int* offset /* = nullptr */) const
{
    unsigned int idx = findLastIndexWhere([&](unsigned int idx) { return cdf_values[idx] <= uniform_sample; },
                                          (unsigned int)(cdf_values.size()));

    if (offset)
        *offset = idx;

    if (pdf_value)
        *pdf_value = values[idx]/integral;

    imp_float shift = uniform_sample - cdf_values[idx];
    imp_float range = cdf_values[idx+1] - cdf_values[idx];

    if (range > 0)
        shift /= range;

    return (idx + shift)/size();
}

unsigned int DistributionFunction1D::discreteSample(imp_float uniform_sample,
                                                    imp_float* pdf_value /* = nullptr */,
                                                    imp_float* shift /* = nullptr */) const
{
    unsigned int idx = findLastIndexWhere([&](unsigned int idx) { return cdf_values[idx] <= uniform_sample; },
                                          (unsigned int)(cdf_values.size()));

    if (pdf_value)
        *pdf_value = discretePDF(idx);

    if (shift)
        *shift = (uniform_sample - cdf_values[idx])/(cdf_values[idx+1] - cdf_values[idx]);

    return idx;
}

// Sampling function definitions

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

Point2F rejectionDiskSample(RandomNumberGenerator& rng)
{
    Point2F sample;

    do
    {
        sample.x = 1 - 2*rng.uniformFloat();
        sample.y = 1 - 2*rng.uniformFloat();

    } while (sample.x*sample.x + sample.y*sample.y > 1.0f);

    return sample;
}

// Given a sample point inside the unit square, returns a uniformly sampled point inside the unit disk
Point2F uniformDiskSample(const Point2F& uniform_sample)
{
    imp_float radius = std::sqrt(uniform_sample.x);
    imp_float theta = uniform_sample.y*IMP_TWO_PI;

    return Point2F(radius*std::cos(theta), radius*std::sin(theta));
}

// Given a sample point inside the unit square, returns a concentric sampled point inside the unit disk
Point2F concentricDiskSample(const Point2F& uniform_sample)
{
    if (uniform_sample.x == 0 && uniform_sample.y == 0)
        return Point2F(0, 0);

    imp_float a = 2*uniform_sample.x - 1;
    imp_float b = 2*uniform_sample.y - 1;

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

// Given a sample point inside the unit square, returns a uniformly sampled direction vector in the unit hemisphere around the z-axis
Vector3F uniformHemisphereSample(const Point2F& uniform_sample)
{
    imp_float cos_theta = uniform_sample.x;
    imp_float sin_theta = std::sqrt(std::max<imp_float>(0, 1 - cos_theta*cos_theta));
    imp_float phi = IMP_TWO_PI*uniform_sample.y;

    return Vector3F(sin_theta*std::cos(phi), sin_theta*std::sin(phi), cos_theta);
}

// Given a sample point inside the unit square, returns a cosine-weighted sampled direction vector in the unit hemisphere around the z-axis
Vector3F cosineWeightedHemisphereSample(const Point2F& uniform_sample)
{
    const Point2F& disk_sample = concentricDiskSample(uniform_sample);

    imp_float z = std::sqrt(std::max<imp_float>(0, 1 - disk_sample.x*disk_sample.x - disk_sample.y*disk_sample.y));

    return Vector3F(disk_sample.x, disk_sample.y, z);
}

} // RayImpact
} // Impact
