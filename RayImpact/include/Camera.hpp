#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include "BoundingRectangle.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "AnimatedTransformation.hpp"
#include "Sensor.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// CameraSample declarations

struct CameraSample
{
    Point2F sensor_point; // Sample position on the camera sensor (in raster space)
    Point2F lens_point; // Sample position on the camera lens
    imp_float time; // Time of the sample (with respect to shutter opening and closing time)
};

// Camera declarations

class Camera {

public:

    AnimatedTransformation camera_to_world; // Transformation from camera space to world space
    const imp_float shutter_opening_time; // Point in time that the shutter opens
    const imp_float shutter_closing_time; // Point in time that the shutter closes
    Sensor* sensor; // The sensor used by the camera
    const Medium* medium; // The medium surrounding the camera

    Camera(const AnimatedTransformation& camera_to_world,
           imp_float shutter_opening_time,
           imp_float shutter_closing_time,
           Sensor* sensor,
           const Medium* medium);

    virtual imp_float generateRay(const CameraSample& sample,
                                  Ray* ray) const = 0;

    virtual imp_float generateRayWithOffsets(const CameraSample& sample,
                                             RayWithOffsets* ray) const;
};

// ProjectiveCamera declarations

class ProjectiveCamera : public Camera {

protected:

    Transformation camera_to_screen; // Transformation from camera space to screen space
    Transformation raster_to_screen; // Transformation from raster space to screen space
    Transformation screen_to_raster; // Transformation from screen space to raster space
    Transformation raster_to_camera; // Transformation from raster space to camera space

    imp_float lens_radius; // Radius of the camera aperture
    imp_float focal_distance; // Distance in camera space at which objects are in focus

public:

    ProjectiveCamera(const AnimatedTransformation& camera_to_world,
                     const Transformation& camera_to_screen,
                     const BoundingRectangleF& screen_window,
                     imp_float shutter_opening_time,
                     imp_float shutter_closing_time,
                     imp_float lens_radius,
                     imp_float focal_distance,
                     Sensor* sensor,
                     const Medium* medium);
};

// Camera inline method definitions

inline Camera::Camera(const AnimatedTransformation& camera_to_world,
					  imp_float shutter_opening_time,
					  imp_float shutter_closing_time,
					  Sensor* sensor,
					  const Medium* medium)
    : camera_to_world(camera_to_world),
      shutter_opening_time(shutter_opening_time),
      shutter_closing_time(shutter_closing_time),
      sensor(sensor),
      medium(medium)
{}

} // RayImpact
} // Impact
