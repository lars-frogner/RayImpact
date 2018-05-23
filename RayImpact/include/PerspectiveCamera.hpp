#pragma once
#include "precision.hpp"
#include "Camera.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// PerspectiveCamera declarations

class PerspectiveCamera : public ProjectiveCamera {

private:

    Vector3F horizontal_pixel_offset; // The horizontal offset between two pixels in camera space
    Vector3F vertical_pixel_offset; // The vertical offset between two pixels in camera space

public:

    PerspectiveCamera(const AnimatedTransformation& camera_to_world,
                      const BoundingRectangleF& screen_window,
                      imp_float shutter_opening_time,
                      imp_float shutter_closing_time,
                      imp_float lens_radius,
                      imp_float focal_distance,
                      imp_float field_of_view,
                      Sensor* sensor,
                      const Medium* medium);

    imp_float generateRay(const CameraSample& sample,
                          Ray* ray) const;

    imp_float generateRayWithOffsets(const CameraSample& sample,
                                     RayWithOffsets* ray) const;
};

// PerspectiveCamera function declarations

Camera* createPerspectiveCamera(const AnimatedTransformation& camera_to_world,
                                Sensor* sensor,
                                const Medium* medium,
                                const ParameterSet& parameters);

// PerspectiveCamera inline method definitions

inline PerspectiveCamera::PerspectiveCamera(const AnimatedTransformation& camera_to_world,
											const BoundingRectangleF& screen_window,
											imp_float shutter_opening_time,
											imp_float shutter_closing_time,
											imp_float lens_radius,
											imp_float focal_distance,
											imp_float field_of_view,
											Sensor* sensor,
											const Medium* medium)
    : ProjectiveCamera::ProjectiveCamera(camera_to_world,
                                         Transformation::perspective(field_of_view, 0.01f, 1000.0f),
                                         screen_window,
                                         shutter_opening_time,
                                         shutter_closing_time,
                                         lens_radius,
                                         focal_distance,
                                         sensor,
                                         medium)
{
    const Point3F& origin = raster_to_camera(Point3F(0, 0, 0));

    horizontal_pixel_offset = raster_to_camera(Point3F(1, 0, 0)) - origin;
    vertical_pixel_offset = raster_to_camera(Point3F(0, 1, 0)) - origin;
}

} // RayImpact
} // Impact
