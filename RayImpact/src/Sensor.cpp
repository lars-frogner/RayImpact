#include "Sensor.hpp"
#include "error.hpp"
#include "memory.hpp"
#include "image_util.hpp"
#include "string_util.hpp"
#include "api.hpp"
#include <algorithm>
#include <cmath>

namespace Impact {
namespace RayImpact {

// Sensor method definitions

Sensor::Sensor(const Vector2I& resolution,
               const BoundingRectangleF& crop_window,
               std::unique_ptr<Filter> reconstruction_filter,
               imp_float diagonal_extent,
               const std::string& output_filename,
               imp_float final_image_scale /* = 1.0f */)
    : full_resolution(resolution),
      diagonal_extent(diagonal_extent),
      filter(std::move(reconstruction_filter)),
      output_filename(output_filename),
      final_image_scale(final_image_scale)
{
    // Convert crop window from NDC space to raster space
    raster_crop_window.lower_corner = Point2I((int)std::ceil(resolution.x*crop_window.lower_corner.x),
                                              (int)std::ceil(resolution.y*crop_window.lower_corner.y));
    raster_crop_window.upper_corner = Point2I((int)std::ceil(resolution.x*crop_window.upper_corner.x),
                                              (int)std::ceil(resolution.y*crop_window.upper_corner.y));

    imp_assert(!raster_crop_window.isDegenerate());

    // Allocate memory for pixels
    pixels = std::unique_ptr<Pixel[]>(new Pixel[raster_crop_window.area()]);

    // Precompute filter values for a range of offsets covering the radii of the filter

    imp_float idx_norm = 1.0f/filter_table_width;

    unsigned int table_idx = 0;

    for (unsigned int j = 0; j < filter_table_width; j++) {
        for (unsigned int i = 0; i < filter_table_width; i++)
        {
            filter_table[table_idx] = filter->evaluate(Point2F((i + 0.5f)*idx_norm*filter->radius.x,
                                                               (j + 0.5f)*idx_norm*filter->radius.y));
            table_idx++;
        }
    }
}

// Returns a bounding rectangle encompassing all pixels on the sensor that need to be sampled
BoundingRectangleI Sensor::samplingBounds() const
{
    Vector2F half_pixel_extent(0.5f, 0.5f);

    const Point2I& lower_bound_for_samples = static_cast<Point2I>(floor(static_cast<Point2F>(raster_crop_window.lower_corner)
                                                                        + half_pixel_extent
                                                                        - filter->radius));

    const Point2I& upper_bound_for_samples = static_cast<Point2I>(ceil(static_cast<Point2F>(raster_crop_window.upper_corner)
                                                                       - half_pixel_extent
                                                                       + filter->radius));

    return BoundingRectangleI(lower_bound_for_samples, upper_bound_for_samples);
}

// Returns a zero-centered bounding rectangle encompassing the physical extent [m] of the sensor
BoundingRectangleF Sensor::physicalExtent() const
{
    imp_float aspect_ratio = (imp_float)full_resolution.y/(imp_float)full_resolution.x;
    imp_float x = std::sqrt(diagonal_extent*diagonal_extent/(1 + aspect_ratio*aspect_ratio));
    imp_float y = x*aspect_ratio;

    return BoundingRectangleF(Point2F(-0.5f*x, -0.5f*y), Point2F(0.5f*x, 0.5f*y));
}

// Returns the sensor region that will recieve contributions from samples in the given bounds
std::unique_ptr<SensorRegion> Sensor::sensorRegion(const BoundingRectangleI& region_sampling_bounds)
{
    Vector2F half_pixel_extent(0.5f, 0.5f);

    // Compute the bounds on pixels that can be affected by samples in the region sampling bounds

    const Point2I& lower_bound_on_affected_pixels = static_cast<Point2I>(ceil(static_cast<Point2F>(region_sampling_bounds.lower_corner)
                                                                              - half_pixel_extent
                                                                              - filter->radius));

    const Point2I& upper_bound_on_affected_pixels = static_cast<Point2I>(floor(static_cast<Point2F>(region_sampling_bounds.upper_corner)
                                                                               - half_pixel_extent
                                                                               + filter->radius)) + Vector2I(1, 1); // Add 1 to ensure exclusive discrete upper bound

    const BoundingRectangleI& region_pixel_bounds = intersectionOf(BoundingRectangleI(lower_bound_on_affected_pixels,
                                                                                      upper_bound_on_affected_pixels),
                                                                   raster_crop_window);

    return std::unique_ptr<SensorRegion>(new SensorRegion(region_pixel_bounds,
                                                          filter->radius,
                                                          filter_table_width,
                                                          filter_table));
}

Sensor::Pixel& Sensor::pixel(const Point2I& pixel_position)
{
    int crop_window_width = raster_crop_window.upper_corner.x - raster_crop_window.lower_corner.x;

    int crop_window_pixel_idx = crop_window_width*(pixel_position.y - raster_crop_window.lower_corner.y)
                                                + (pixel_position.x - raster_crop_window.lower_corner.x);

    return pixels[crop_window_pixel_idx];
}

// Takes a sensor region and merges its raw pixels into the full sensor pixel array
void Sensor::mergeSensorRegion(std::unique_ptr<SensorRegion> sensor_region)
{
    // Aquire lock to the mutex so that only one thread can merge pixels at a time
    std::lock_guard<std::mutex> lock(mutex);

    // Iterate over all pixels in the region
    for (Point2I pixel_position : sensor_region->pixelBounds())
    {
        const RawPixel& raw_pixel = sensor_region->rawPixel(pixel_position);
        Pixel& merge_pixel = pixel(pixel_position);

        // Convert the total pixel radiance at the pixel to tristimulus color values
        imp_float xyz[3];
        raw_pixel.recieved_energy.computeTristimulusValues(xyz);

        // Add colors in the region pixel to the sensor pixel
        merge_pixel.xyz_values[0] += xyz[0];
        merge_pixel.xyz_values[1] += xyz[1];
        merge_pixel.xyz_values[2] += xyz[2];

        // Update sum of filter weights
        merge_pixel.sum_of_filter_weights += raw_pixel.sum_of_filter_weights;
    }
}

// Assigns the given energy spectra to the sensor pixels in the crop window
void Sensor::setPixels(const EnergySpectrum* pixel_values)
{
    unsigned int n_pixels = raster_crop_window.area();

    for (unsigned int i = 0; i < n_pixels; i++)
    {
        Pixel& pixel = pixels[i];

        pixel_values[i].computeTristimulusValues(pixel.xyz_values);

        pixel.sum_of_filter_weights = 1.0f;

        pixel.xyz_sums_of_splats[0] = 0;
        pixel.xyz_sums_of_splats[1] = 0;
        pixel.xyz_sums_of_splats[2] = 0;
    }
}

void Sensor::addSplat(const Point2F& sample_position,
                      const RadianceSpectrum& radiance)
{
    if (!raster_crop_window.containsExclusive(static_cast<Point2I>(sample_position)))
        return;

    imp_float xyz[3];

    radiance.computeTristimulusValues(xyz);

    Pixel& sample_pixel = pixel(static_cast<Point2I>(sample_position));

    sample_pixel.xyz_sums_of_splats[0].add(xyz[0]);
    sample_pixel.xyz_sums_of_splats[1].add(xyz[1]);
    sample_pixel.xyz_sums_of_splats[2].add(xyz[2]);
}

void Sensor::writeImage(imp_float splat_scale /* = 1 */)
{
    std::unique_ptr<imp_float[]> pixel_rgb_values(new imp_float[3*raster_crop_window.area()]);

    unsigned int pixel_idx = 0;

    // Iterate through all pixels in the crop window
    for (Point2I pixel_position : raster_crop_window)
    {
        Pixel& sensor_pixel = pixel(pixel_position);

        // Convert tristimulus XYZ values to RGB values
        tristimulusToRGB(sensor_pixel.xyz_values, &pixel_rgb_values[3*pixel_idx]);

        // Perform normalization to account for the accumulation of multiple weighted samples
        if (sensor_pixel.sum_of_filter_weights != 0)
        {
            imp_float norm = 1.0f/sensor_pixel.sum_of_filter_weights;

            pixel_rgb_values[3*pixel_idx    ] = std::max<imp_float>(0, pixel_rgb_values[3*pixel_idx    ]*norm);
            pixel_rgb_values[3*pixel_idx + 1] = std::max<imp_float>(0, pixel_rgb_values[3*pixel_idx + 1]*norm);
            pixel_rgb_values[3*pixel_idx + 2] = std::max<imp_float>(0, pixel_rgb_values[3*pixel_idx + 2]*norm);
        }

        // Add color contributions from splats

        imp_float splat_xyz[3] = {sensor_pixel.xyz_sums_of_splats[0],
                                  sensor_pixel.xyz_sums_of_splats[1],
                                  sensor_pixel.xyz_sums_of_splats[2]};

        imp_float splat_rgb[3];

        tristimulusToRGB(splat_xyz, splat_rgb);

        pixel_rgb_values[3*pixel_idx    ] += splat_rgb[0]*splat_scale;
        pixel_rgb_values[3*pixel_idx + 1] += splat_rgb[1]*splat_scale;
        pixel_rgb_values[3*pixel_idx + 2] += splat_rgb[2]*splat_scale;

        pixel_idx++;
    }

    writePFM(output_filename, &pixel_rgb_values[0],
             raster_crop_window.upper_corner.x - raster_crop_window.lower_corner.x,
             raster_crop_window.upper_corner.y - raster_crop_window.lower_corner.y,
			 final_image_scale);
}

// SensorSection method definitions

// Finds the pixels affected by the given sample and gives them their corresponding radiance contribution
void SensorRegion::addSample(const Point2F& sample_position,
                             const RadianceSpectrum& radiance,
                             imp_float sample_weight /* = 1 */)
{
    // Convert continuous sample position to the space of discrete pixel positions by subtracting 0.5
    const Point2F discrete_sample_position = sample_position - Vector2F(0.5f, 0.5f);

    // Compute the bounds on pixels that can be affected by the sample

    Point2I lower_bound_on_affected_pixels = static_cast<Point2I>(ceil(discrete_sample_position - filter_radius));
    Point2I upper_bound_on_affected_pixels = static_cast<Point2I>(floor(discrete_sample_position + filter_radius)) + Vector2I(1, 1); // Add 1 to ensure exclusive discrete upper bound

    lower_bound_on_affected_pixels = max(lower_bound_on_affected_pixels, pixel_bounds.lower_corner);
    upper_bound_on_affected_pixels = min(upper_bound_on_affected_pixels, pixel_bounds.upper_corner);

    // Precompute coordinates into the filter table for given discrete pixel offsets

    int* filter_table_coords_x = allocated_on_stack(int, upper_bound_on_affected_pixels.x - lower_bound_on_affected_pixels.x);
    int* filter_table_coords_y = allocated_on_stack(int, upper_bound_on_affected_pixels.y - lower_bound_on_affected_pixels.y);

    for (int x = lower_bound_on_affected_pixels.x; x < upper_bound_on_affected_pixels.x; x++)
    {
        imp_float coord = std::abs((imp_float)x - discrete_sample_position.x)*inverse_filter_radius.x*filter_table_width;
        filter_table_coords_x[x - lower_bound_on_affected_pixels.x] = std::min((int)std::floor(coord), (int)filter_table_width - 1);
    }

    for (int y = lower_bound_on_affected_pixels.y; y < upper_bound_on_affected_pixels.y; y++)
    {
        imp_float coord = std::abs(((imp_float)y - discrete_sample_position.y)*inverse_filter_radius.y*filter_table_width);
        filter_table_coords_y[y - lower_bound_on_affected_pixels.y] = std::min((int)std::floor(coord), (int)filter_table_width - 1);
    }

    // Add sample contributions to affected pixels
    for (int y = lower_bound_on_affected_pixels.y; y < upper_bound_on_affected_pixels.y; y++) {
        for (int x = lower_bound_on_affected_pixels.x; x < upper_bound_on_affected_pixels.x; x++)
        {
            int filter_table_idx = filter_table_width*filter_table_coords_y[y - lower_bound_on_affected_pixels.y]
                                                    + filter_table_coords_x[x - lower_bound_on_affected_pixels.x];

            imp_float filter_weight = filter_table[filter_table_idx];

            RawPixel& pixel = rawPixel(Point2I(x, y));

            pixel.recieved_energy += radiance*sample_weight*filter_weight;
            pixel.sum_of_filter_weights += filter_weight;
        }
    }
}

const RawPixel& SensorRegion::rawPixel(const Point2I& pixel_position) const
{
    int region_width = pixel_bounds.upper_corner.x - pixel_bounds.lower_corner.x;

    int region_pixel_idx = region_width*(pixel_position.y - pixel_bounds.lower_corner.y)
                                      + (pixel_position.x - pixel_bounds.lower_corner.x);

    return pixels[region_pixel_idx];
}

RawPixel& SensorRegion::rawPixel(const Point2I& pixel_position)
{
    int region_width = pixel_bounds.upper_corner.x - pixel_bounds.lower_corner.x;

    int region_pixel_idx = region_width*(pixel_position.y - pixel_bounds.lower_corner.y)
                                      + (pixel_position.x - pixel_bounds.lower_corner.x);

    return pixels[region_pixel_idx];
}

// Sensor function definitions

Sensor* createImageSensor(std::unique_ptr<Filter> filter,
                          const std::string& output_filename,
                          const ParameterSet& parameters)
{
    unsigned int n_values;

    Vector2I resolution;

    const int* resolution_values = parameters.getIntValues("resolution", &n_values);

    if (!resolution_values || n_values != 2)
    {
        resolution.x = 400;
        resolution.y = 400;

        if (resolution_values)
            printWarningMessage("the sensor \"resolution\" parameter must consist of exactly two integers. Using default resolution.");
    }
    else
    {
        resolution.x = std::abs(resolution_values[0]);
        resolution.y = std::abs(resolution_values[1]);
    }

    BoundingRectangleF crop_window;

    const Vector2F* crop_window_corners = parameters.getPairValues("crop_window", &n_values);

    if (!crop_window_corners || n_values != 2)
    {
        crop_window.lower_corner = Point2F(0, 0);
        crop_window.upper_corner = Point2F(1, 1);

        if (crop_window_corners)
            printWarningMessage("the sensor \"crop_window\" parameter must consist of exactly two point2f values. Using default crop window.");
    }
    else
    {
        crop_window = crop_window.aroundPoints(static_cast<Point2F>(crop_window_corners[0]), static_cast<Point2F>(crop_window_corners[1]));
    }

    imp_float diagonal_size = parameters.getSingleFloatValue("diagonal_size", 50.0f);
    imp_float pixel_scaling = parameters.getSingleFloatValue("pixel_scaling", 1.0f);
	
	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Camera sensor:"
						 "\n    %-20s%d x %d px"
						 "\n    %-20s%s"
						 "\n    %-20s%g mm"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Resolution:", resolution.x, resolution.y,
						 "Crop window:", crop_window.toString().c_str(),
						 "Diagonal size:", diagonal_size,
						 "Pixel scaling:", (pixel_scaling < 0)? "auto" : formatString("%g", pixel_scaling).c_str(),
						 "Output file:", output_filename.c_str());
	}

    return new Sensor(resolution,
                      crop_window,
                      std::move(filter),
                      diagonal_size*1e-3f,
                      output_filename,
                      pixel_scaling);
}

} // RayImpact
} // Impact
