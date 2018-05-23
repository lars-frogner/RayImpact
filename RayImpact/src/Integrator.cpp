#include "Integrator.hpp"
#include "precision.hpp"
#include "parallel.hpp"
#include "geometry.hpp"
#include "BoundingRectangle.hpp"
#include "Sensor.hpp"
#include "BSDF.hpp"
#include <algorithm>
#include <cmath>

namespace Impact {
namespace RayImpact {

// SampleIntegrator method definitions

RadianceSpectrum SampleIntegrator::specularlyReflectedRadiance(const RayWithOffsets& outgoing_ray,
                                                               const SurfaceScatteringEvent& scattering_event,
                                                               const Scene& scene,
                                                               Sampler& sampler,
                                                               RegionAllocator& allocator,
                                                               unsigned int scattering_count) const
{
    const Vector3F& outgoing_direction = scattering_event.outgoing_direction;
    Vector3F incident_direction;

    imp_float pdf_value;

    BXDFType type = BXDFType(BSDF_REFLECTION | BSDF_SPECULAR);

    const Spectrum& bsdf_value = scattering_event.bsdf->sample(outgoing_direction,
                                                               &incident_direction,
                                                               sampler.next2DSampleComponent(),
                                                               &pdf_value,
                                                               type);

    const Normal3F& shading_normal = scattering_event.shading.surface_normal;

    imp_float abs_cos_theta_incident = incident_direction.absDot(shading_normal);

    if (pdf_value > 0 && !bsdf_value.isBlack() && abs_cos_theta_incident != 0)
    {
        RayWithOffsets incident_ray = scattering_event.spawnRay(incident_direction);

        if (outgoing_ray.has_offsets)
        {
            incident_ray.has_offsets = true;

            incident_ray.x_offset_ray_origin = scattering_event.position + scattering_event.dpdx;
            incident_ray.y_offset_ray_origin = scattering_event.position + scattering_event.dpdy;

            const Normal3F& dndx = scattering_event.shading.dndu*scattering_event.dudx +
                                   scattering_event.shading.dndv*scattering_event.dvdx;

            const Normal3F& dndy = scattering_event.shading.dndu*scattering_event.dudy +
                                   scattering_event.shading.dndv*scattering_event.dvdy;

            const Vector3F& dwodx = -outgoing_ray.x_offset_ray_direction - outgoing_direction;
            const Vector3F& dwody = -outgoing_ray.y_offset_ray_direction - outgoing_direction;

            imp_float dwodotndx = dwodx.dot(shading_normal) + outgoing_direction.dot(dndx);
            imp_float dwodotndy = dwody.dot(shading_normal) + outgoing_direction.dot(dndy);

            imp_float cos_theta_outgoing = outgoing_direction.dot(shading_normal);

            incident_ray.x_offset_ray_direction = incident_direction -
                                                  dwodx +
                                                  2.0f*Vector3F(cos_theta_outgoing*dndx + dwodotndx*shading_normal);

            incident_ray.y_offset_ray_direction = incident_direction -
                                                  dwody +
                                                  2.0f*Vector3F(cos_theta_outgoing*dndy + dwodotndy*shading_normal);
        }

        return bsdf_value*
               incidentRadiance(incident_ray, scene, sampler, allocator, scattering_count + 1)*
               (abs_cos_theta_incident/pdf_value);
    }
    else
        return Spectrum(0.0f);
}

RadianceSpectrum SampleIntegrator::specularlyTransmittedRadiance(const RayWithOffsets& outgoing_ray,
                                                                 const SurfaceScatteringEvent& scattering_event,
                                                                 const Scene& scene,
                                                                 Sampler& sampler,
                                                                 RegionAllocator& allocator,
                                                                 unsigned int scattering_count) const
{
    const Vector3F& outgoing_direction = scattering_event.outgoing_direction;
    Vector3F incident_direction;

    imp_float pdf_value;

    BXDFType type = BXDFType(BSDF_TRANSMISSION | BSDF_SPECULAR);

    const Spectrum& bsdf_value = scattering_event.bsdf->sample(outgoing_direction,
                                                               &incident_direction,
                                                               sampler.next2DSampleComponent(),
                                                               &pdf_value,
                                                               type);

    const Normal3F& shading_normal = scattering_event.shading.surface_normal;

    imp_float abs_cos_theta_incident = incident_direction.absDot(shading_normal);

    if (pdf_value > 0 && !bsdf_value.isBlack() && abs_cos_theta_incident != 0)
    {
        RayWithOffsets incident_ray = scattering_event.spawnRay(incident_direction);

        if (outgoing_ray.has_offsets)
        {
            incident_ray.has_offsets = true;

            incident_ray.x_offset_ray_origin = scattering_event.position + scattering_event.dpdx;
            incident_ray.y_offset_ray_origin = scattering_event.position + scattering_event.dpdy;

            const Normal3F& dndx = scattering_event.shading.dndu*scattering_event.dudx +
                                   scattering_event.shading.dndv*scattering_event.dvdx;

            const Normal3F& dndy = scattering_event.shading.dndu*scattering_event.dudy +
                                   scattering_event.shading.dndv*scattering_event.dvdy;

            const Vector3F& dwodx = -outgoing_ray.x_offset_ray_direction - outgoing_direction;
            const Vector3F& dwody = -outgoing_ray.y_offset_ray_direction - outgoing_direction;

            imp_float dwodotndx = dwodx.dot(shading_normal) + outgoing_direction.dot(dndx);
            imp_float dwodotndy = dwody.dot(shading_normal) + outgoing_direction.dot(dndy);

            imp_float cos_theta_outgoing = outgoing_direction.dot(shading_normal);
            imp_float cos_theta_incident = incident_direction.dot(shading_normal);

            imp_float refractive_index = scattering_event.bsdf->refractive_index_outside;

            if (cos_theta_outgoing < 0)
                refractive_index = 1.0f/refractive_index;

            imp_float mu = -refractive_index*cos_theta_outgoing - cos_theta_incident;

            imp_float dmu_fac = refractive_index + refractive_index*refractive_index*cos_theta_outgoing/cos_theta_incident;

            imp_float dmudx = dmu_fac*dwodotndx;
            imp_float dmudy = dmu_fac*dwodotndy;

            incident_ray.x_offset_ray_direction = incident_direction +
                                                  refractive_index*dwodx -
                                                  Vector3F(mu*dndx + dmudx*shading_normal);

            incident_ray.y_offset_ray_direction = incident_direction +
                                                  refractive_index*dwody -
                                                  Vector3F(mu*dndy + dmudy*shading_normal);
        }

        return bsdf_value*
               incidentRadiance(incident_ray, scene, sampler, allocator, scattering_count + 1)*
               (abs_cos_theta_incident/pdf_value);
    }
    else
        return Spectrum(0.0f);
}

void SampleIntegrator::renderSinglePixel(const Scene& scene, const Point2I& single_pixel)
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
			if (pixel == single_pixel)
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
        }

        // Merge the sensor region into the full sensor
        camera->sensor->mergeSensorRegion(std::move(sensor_region));
    },
    n_sensor_regions_x, n_sensor_regions_y);

    // Write the final image to file
    camera->sensor->writeImage();
}

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
