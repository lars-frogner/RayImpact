#include "PerspectiveCamera.hpp"
#include "sampling.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// PerspectiveCamera method definitions

imp_float PerspectiveCamera::generateRay(const CameraSample& sample,
                                         Ray* ray) const
{
    Point3F sensor_point_in_raster_space(sample.sensor_point.x, sample.sensor_point.y, 0);

    // Find sensor point in camera space
    const Point3F& sensor_point = raster_to_camera(sensor_point_in_raster_space);

    // Ray starts at the origin and points towards the sensor point at the near plane in camera space
    *ray = Ray(Point3F(0, 0, 0), Vector3F(sensor_point).normalized());

    if (lens_radius > 0)
    {
        // Map [0, 1) square sample to sample on the circular lens
        const Point2F& lens_point = lens_radius*concentricDiskSample(sample.lens_point);

        // Find where the ray through the lens center intersects the plane of focus
        imp_float intersection_distance_with_plane_of_focus = -focal_distance/ray->direction.z;
        const Point3F& intersection_point_with_plane_of_focus = (*ray)(intersection_distance_with_plane_of_focus);

        // Update ray to account for lens
        ray->origin = Point3F(lens_point.x, lens_point.y, 0);
        ray->direction = (intersection_point_with_plane_of_focus - ray->origin).normalized();
    }

    ray->time = ::Impact::lerp(shutter_opening_time, shutter_closing_time, sample.time);
    ray->medium = medium;

    // Transform to world space
    *ray = camera_to_world(*ray);

    return 1.0f;
}

imp_float PerspectiveCamera::generateRayWithOffsets(const CameraSample& sample,
                                                    RayWithOffsets* ray) const
{
    Point3F sensor_point_in_raster_space(sample.sensor_point.x, sample.sensor_point.y, 0);

    // Find sensor point in camera space
    const Point3F& sensor_point = raster_to_camera(sensor_point_in_raster_space);

    // Ray starts at the origin and points towards the sensor point at the near plane in camera space
    *ray = RayWithOffsets(Point3F(0, 0, 0), Vector3F(sensor_point).normalized());

    if (lens_radius > 0)
    {
        // Map [0, 1) square sample to sample on the circular lens
        const Point2F& lens_point = lens_radius*concentricDiskSample(sample.lens_point);

        // Find where the ray through the lens center intersects the plane of focus
        imp_float intersection_distance_with_plane_of_focus = -focal_distance/ray->direction.z;
        Point3F intersection_point_with_plane_of_focus = (*ray)(intersection_distance_with_plane_of_focus);

        // Update ray to account for lens
        ray->origin = Point3F(lens_point.x, lens_point.y, 0);
        ray->direction = (intersection_point_with_plane_of_focus - ray->origin).normalized();

        // Compute direction for x-offset ray through the lens center
        const Vector3F& x_offset_direction = Vector3F(sensor_point + horizontal_pixel_offset).normalized();

        // Compute intersection distance for the x-offset ray
        intersection_distance_with_plane_of_focus = -focal_distance/x_offset_direction.z;

        // Compute intersection point for the x-offset ray
        intersection_point_with_plane_of_focus = Point3F(0, 0, 0) + x_offset_direction*intersection_distance_with_plane_of_focus;

        ray->x_offset_ray_origin = ray->origin;
        ray->x_offset_ray_direction = (intersection_point_with_plane_of_focus - ray->x_offset_ray_origin).normalized();

        // Compute direction for y-offset ray through the lens center
        const Vector3F& y_offset_direction = Vector3F(sensor_point + vertical_pixel_offset).normalized();

        // Compute intersection distance for the y-offset ray
        intersection_distance_with_plane_of_focus = -focal_distance/y_offset_direction.z;

        // Compute intersection point for the y-offset ray
        intersection_point_with_plane_of_focus = Point3F(0, 0, 0) + y_offset_direction*intersection_distance_with_plane_of_focus;

        ray->y_offset_ray_origin = ray->origin;
        ray->y_offset_ray_direction = (intersection_point_with_plane_of_focus - ray->y_offset_ray_origin).normalized();
    }
    else
    {
        ray->x_offset_ray_origin = ray->origin;
        ray->x_offset_ray_direction = (Vector3F(sensor_point) + horizontal_pixel_offset).normalized();

        ray->y_offset_ray_origin = ray->origin;
        ray->y_offset_ray_direction = (Vector3F(sensor_point) + vertical_pixel_offset).normalized();
    }

    ray->has_offsets = true;
    ray->time = ::Impact::lerp(shutter_opening_time, shutter_closing_time, sample.time);
    ray->medium = medium;

    // Transform to world space
    *ray = camera_to_world(*ray);

    return 1.0f;
}

// PerspectiveCamera function definitions

Camera* createPerspectiveCamera(const AnimatedTransformation& camera_to_world,
                                Sensor* sensor,
                                const Medium* medium,
                                const ParameterSet& parameters)
{
    imp_float shutter_start = parameters.getSingleFloatValue("shutter_start", 0.0f);
    imp_float shutter_end = parameters.getSingleFloatValue("shutter_end", 0.002f);
    imp_float aperture = parameters.getSingleFloatValue("aperture", 0.0f);
    imp_float focal_distance = parameters.getSingleFloatValue("focal_distance", 1.0f);
    imp_float field_of_view = parameters.getSingleFloatValue("field_of_view", 45.0f);

	imp_float aspect = sensor->full_resolution.y/static_cast<imp_float>(sensor->full_resolution.x);
	
	BoundingRectangleF screen_window(Point2F(-1, -aspect), Point2F(1, aspect));

	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Camera:"
						 "\n    %-20s%s"
						 "\n    %-20s%g s"
						 "\n    %-20s%g mm"
						 "\n    %-20s%g m"
						 "\n    %-20s%g degrees"
						 "\n    %-20s%s m"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Type:", (aperture == 0)? "Perspective (pinhole)" : "Perspective",
						 "Shutter:", shutter_end - shutter_start,
						 "Aperture:", aperture,
						 "Focal distance:", focal_distance,
						 "Field of view:", field_of_view,
						 "Position:", camera_to_world(Point3F(0, 0, 0), shutter_start).toString().c_str(),
						 "Look direction:", camera_to_world(Vector3F(0, 0, -1), shutter_start).toString().c_str(),
						 "Up direction:", camera_to_world(Vector3F(0, 1, 0), shutter_start).toString().c_str());
	}

    return new PerspectiveCamera(camera_to_world,
                                 screen_window,
                                 shutter_start,
                                 shutter_end,
                                 aperture*5e-4f,
                                 focal_distance,
                                 field_of_view,
                                 sensor,
                                 medium);
}

} // RayImpact
} // Impact
