#pragma once
#include "precision.hpp"
#include "AtomicFloat.hpp"
#include "geometry.hpp"
#include "BoundingRectangle.hpp"
#include "Filter.hpp"
#include "Spectrum.hpp"
#include "ParameterSet.hpp"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace Impact {
namespace RayImpact {

// Forward declarations
class SensorRegion;
struct RawPixel;

// Sensor declarations

class Sensor {

private:

    struct Pixel
    {
        imp_float xyz_values[3] = {0.0f, 0.0f, 0.0f}; // Tristrimulus color values for the pixel
        imp_float sum_of_filter_weights = 0; // Sum of filter weights for samples contributing to the pixel
        AtomicFloat xyz_sums_of_splats[3]; // Sums of the tristimulus color values accumulated by splats
        imp_float pad; // Extra variable to ensure alignment in memory
    };

    std::unique_ptr<Pixel[]> pixels; // The sensor pixels

    const imp_float final_image_scale; // Scale factor to apply to the final image before writing to file

    static constexpr unsigned int filter_table_width = 16; // Width of the table of precomputed filter values
    imp_float filter_table[filter_table_width*filter_table_width]; // Table of precomputed filter values

    std::mutex mutex; // Mutex for avoiding simultaneous modifications of the sensor pixels by different threads

    Pixel& pixel(const Point2I& pixel_position);

public:

    const Vector2I full_resolution; // Total number of pixels along each dimension of the sensor
    BoundingRectangleI raster_crop_window; // Bounds in raster space of the window of pixles to render
    const imp_float diagonal_extent; // Physical length of the sensor diagonal
    std::unique_ptr<Filter> filter; // The filter to use for image reconstruction
    const std::string output_filename; // Filename to write the output image to

    Sensor(const Vector2I& resolution,
           const BoundingRectangleF& crop_window,
           std::unique_ptr<Filter> reconstruction_filter,
           imp_float diagonal_extent,
           const std::string& output_filename,
           imp_float final_image_scale = 1);

    BoundingRectangleI samplingBounds() const;

    BoundingRectangleF physicalExtent() const;

    std::unique_ptr<SensorRegion> sensorRegion(const BoundingRectangleI& region_sampling_bounds);

    void mergeSensorRegion(std::unique_ptr<SensorRegion> sensor_region);

    void setPixels(const EnergySpectrum* pixel_values);

    void addSplat(const Point2F& sample_position,
                  const RadianceSpectrum& radiance);

    void writeImage(imp_float splat_scale = 1);
};

// SensorSection declarations

class SensorRegion {

private:

    const BoundingRectangleI pixel_bounds; // Boundary rectangle encompassing all pixels in the sensor region
    const Vector2F filter_radius; // Radius vector of the filter used by the sensor
    const Vector2F inverse_filter_radius; // Reciprocal of the radius vector
    const unsigned int filter_table_width; // The width of the filter table used by the sensor
    const imp_float* filter_table; // The table of filter values used by the sensor

    std::vector<RawPixel> pixels; // The pixels contained in the sensor region

public:

    SensorRegion(const BoundingRectangleI& pixel_bounds,
                 const Vector2F& filter_radius,
                 const unsigned int filter_table_width,
                 const imp_float* filter_table);

    void addSample(const Point2F& sample_position,
                   const RadianceSpectrum& radiance,
                   imp_float sample_weight = 1);

    const BoundingRectangleI& pixelBounds() const;

    const RawPixel& rawPixel(const Point2I& pixel_position) const;

    RawPixel& rawPixel(const Point2I& pixel_position);
};

struct RawPixel
{
    EnergySpectrum recieved_energy = 0.0f; // Sum of weighted radiance contributions
    imp_float sum_of_filter_weights = 0; // Sum of filter weights for samples contributing to the pixel
};

// Sensor function declarations

Sensor* createImageSensor(std::unique_ptr<Filter> filter,
                          const std::string& output_filename,
                          const ParameterSet& parameters);

// SensorSection inline method definitions

inline SensorRegion::SensorRegion(const BoundingRectangleI& pixel_bounds,
								  const Vector2F& filter_radius,
								  const unsigned int filter_table_width,
								  const imp_float* filter_table)
    : pixel_bounds(pixel_bounds),
      filter_radius(filter_radius),
      inverse_filter_radius(1.0f/filter_radius.x, 1.0f/filter_radius.y),
      filter_table_width(filter_table_width),
      filter_table(filter_table),
      pixels(std::max(0, pixel_bounds.area()))
{}

inline const BoundingRectangleI& SensorRegion::pixelBounds() const
{
    return pixel_bounds;
}

} // RayImpact
} // Impact
