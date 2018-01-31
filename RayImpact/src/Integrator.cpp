#include "Integrator.hpp"
#include "precision.hpp"
#include "parallel.hpp"
#include "geometry.hpp"
#include "BoundingRectangle.hpp"
#include "Sensor.hpp"
#include <algorithm>
#include <cmath>

namespace Impact {
namespace RayImpact {

// SampleIntegrator method implementations

SampleIntegrator::SampleIntegrator(std::shared_ptr<const Camera> camera,
                                   std::shared_ptr<Sampler> sampler)
    : camera(camera), sampler(sampler)
{}

void SampleIntegrator::preprocess(const Scene& scene, Sampler& sampler)
{}

void SampleIntegrator::render(const Scene& scene)
{
    preprocess(scene, *sampler);

    // Compute the number of sensor regions to use in each direction

    const unsigned int sensor_region_extent = 16;

    const BoundingRectangleI& sensor_sampling_bounds = camera->sensor->samplingBounds();
    const Vector2I& sampling_extents = sensor_sampling_bounds.diagonal();

    uint32_t n_sensor_regions_x = (sampling_extents.x + sensor_region_extent - 1)/sensor_region_extent;
    uint32_t n_sensor_regions_y = (sampling_extents.y + sensor_region_extent - 1)/sensor_region_extent;

    // Loop over each sensor region in parallel
    parallelFor2D(
    [&](uint32_t region_i, uint32_t region_j)
    {
        // Create thread-private allocator
        RegionAllocator allocator;
        
        // Create thread-private sampler
        unsigned int seed = region_j*n_sensor_regions_x + region_i;
        std::unique_ptr<Sampler> region_sampler = sampler->cloned(seed);

        // Compute the sampling bounds for the current sensor region

        Point2I lower_region_sampling_bounds(sensor_sampling_bounds.lower_corner.x + region_i*sensor_region_extent,
                                             sensor_sampling_bounds.lower_corner.y + region_j*sensor_region_extent);
        
        Point2I upper_region_sampling_bounds(std::min<int>(lower_region_sampling_bounds.x + sensor_region_extent, sensor_sampling_bounds.upper_corner.x),
                                             std::min<int>(lower_region_sampling_bounds.y + sensor_region_extent, sensor_sampling_bounds.upper_corner.y));

        BoundingRectangleI region_sampling_bounds(lower_region_sampling_bounds, upper_region_sampling_bounds);

        // Obtain the corresponding SensorRegion object from the sensor
        std::unique_ptr<SensorRegion> sensor_region = camera->sensor->sensorRegion(region_sampling_bounds);

        // Loop over the pixels in the region
        for (Point2I pixel : region_sampling_bounds)
        {
            // Set current pixel for the sampler
            region_sampler->setPixel(pixel);

            // Loop over all samples provided by the sampler
            do
            {
                // Generate sample containing a point on the sensor, a point on the lens and a time
                const CameraSample& camera_sample = region_sampler->generateCameraSample(pixel);

                // Get the eye ray for the camera sample
                RayWithOffsets eye_ray;
                imp_float ray_weight = camera->generateRayWithOffsets(camera_sample, &eye_ray);
                eye_ray.scaleOffsets(1.0f/std::sqrt((imp_float)region_sampler->n_samples_per_pixel));

                // Compute the radiance incident on the sensor sample point
                RadianceSpectrum incident_radiance(0.0f);
                if (ray_weight > 0)
                    incident_radiance = incidentRadiance(eye_ray, scene, *region_sampler, allocator);

                // Check for invalid spectrum components

                // Add the sampled radiance to the sensor region
                sensor_region->addSample(camera_sample.sensor_point, incident_radiance, ray_weight);

                // Free memory allocated with the region allocator
                allocator.release();

            } while (region_sampler->beginNextSample());
        }

        // Merge the sensor region into the full sensor
        camera->sensor->mergeSensorRegion(std::move(sensor_region));
    },
    n_sensor_regions_x, n_sensor_regions_y);

    // Write the final image to file
    camera->sensor->writeImage();
}

} // RayImpact
} // Impact
