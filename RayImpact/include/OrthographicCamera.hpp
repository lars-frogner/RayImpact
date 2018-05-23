#pragma once
#include "precision.hpp"
#include "Camera.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// OrthographicCamera declarations

class OrthographicCamera : public ProjectiveCamera {

private:

    Vector3F horizontal_pixel_offset; // The horizontal offset between two pixels in camera space
    Vector3F vertical_pixel_offset; // The vertical offset between two pixels in camera space

public:

    OrthographicCamera(const AnimatedTransformation& camera_to_world,
                       const BoundingRectangleF& screen_window,
                       imp_float shutter_opening_time,
                       imp_float shutter_closing_time,
                       imp_float lens_radius,
                       imp_float focal_distance,
                       Sensor* sensor,
                       const Medium* medium);

    imp_float generateRay(const CameraSample& sample,
                          Ray* ray) const;

    imp_float generateRayWithOffsets(const CameraSample& sample,
                                     RayWithOffsets* ray) const;
};

// OrthographicCamera function declarations

Camera* createOrthographicCamera(const AnimatedTransformation& camera_to_world,
                                 Sensor* sensor,
                                 const Medium* medium,
                                 const ParameterSet& parameters);

// OrthographicCamera inline method definitions

inline OrthographicCamera::OrthographicCamera(const AnimatedTransformation& camera_to_world,
											  const BoundingRectangleF& screen_window,
											  imp_float shutter_opening_time,
											  imp_float shutter_closing_time,
											  imp_float lens_radius,
											  imp_float focal_distance,
											  Sensor* sensor,
											  const Medium* medium)
    : ProjectiveCamera::ProjectiveCamera(camera_to_world,
                                         Transformation::orthographic(0, 1),
                                         screen_window,
                                         shutter_opening_time,
                                         shutter_closing_time,
                                         lens_radius,
                                         focal_distance,
                                         sensor,
                                         medium)
{
    horizontal_pixel_offset = raster_to_camera(Vector3F(1, 0, 0));
    vertical_pixel_offset = raster_to_camera(Vector3F(0, 1, 0));
}

} // RayImpact
} // Impact
