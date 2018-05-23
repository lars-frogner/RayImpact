#include "OrthographicCamera.hpp"
#include "sampling.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// OrthographicCamera method definitions

imp_float OrthographicCamera::generateRay(const CameraSample& sample,
                                          Ray* ray) const
{
    Point3F sensor_point_in_raster_space(sample.sensor_point.x, sample.sensor_point.y, 0);

    // Find sensor point in camera space
    const Point3F& sensor_point = raster_to_camera(sensor_point_in_raster_space);

    // Ray starts at the sensor sample point and points in the negative z-direction in camera space
    *ray = Ray(sensor_point, Vector3F(0, 0, -1));

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

imp_float OrthographicCamera::generateRayWithOffsets(const CameraSample& sample,
                                                     RayWithOffsets* ray) const
{
    Point3F sensor_point_in_raster_space(sample.sensor_point.x, sample.sensor_point.y, 0);

    // Find sensor point in camera space
    const Point3F& sensor_point = raster_to_camera(sensor_point_in_raster_space);

    // Ray starts at the sensor sample point and points in the negative z-direction in camera space
    *ray = RayWithOffsets(sensor_point, Vector3F(0, 0, -1));

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

        // Compute new intersection distance with the new ray
        intersection_distance_with_plane_of_focus = -focal_distance/ray->direction.z;

        // Compute new intersection point for the x-offset ray
        intersection_point_with_plane_of_focus = sensor_point + horizontal_pixel_offset + Vector3F(0, 0, -intersection_distance_with_plane_of_focus);

        ray->x_offset_ray_origin = ray->origin;
        ray->x_offset_ray_direction = (intersection_point_with_plane_of_focus - ray->x_offset_ray_origin).normalized();

        // Compute new intersection point for the y-offset ray
        intersection_point_with_plane_of_focus = sensor_point + vertical_pixel_offset + Vector3F(0, 0, -intersection_distance_with_plane_of_focus);

        ray->y_offset_ray_origin = ray->origin;
        ray->y_offset_ray_direction = (intersection_point_with_plane_of_focus - ray->y_offset_ray_origin).normalized();
    }
    else
    {
        ray->x_offset_ray_origin = ray->origin + horizontal_pixel_offset;
        ray->x_offset_ray_direction = ray->direction;

        ray->y_offset_ray_origin = ray->origin + vertical_pixel_offset;
        ray->y_offset_ray_direction = ray->direction;
    }

    ray->has_offsets = true;
    ray->time = ::Impact::lerp(shutter_opening_time, shutter_closing_time, sample.time);
    ray->medium = medium;

    // Transform to world space
    *ray = camera_to_world(*ray);

    return 1.0f;
}

// OrthographicCamera function definitions

Camera* createOrthographicCamera(const AnimatedTransformation& camera_to_world,
                                 Sensor* sensor,
                                 const Medium* medium,
                                 const ParameterSet& parameters)
{
    /*unsigned int n_values;
    const Point2F* screen_window_corners = parameters.getPoint2FValues("screen_window", &n_values);

    if (!screen_window_corners || n_values != 2)
    {
        screen_window.lower_corner = Point2F(-1, -1);
        screen_window.upper_corner = Point2F(1, 1);

        if (screen_window_corners)
            printWarningMessage("the camera \"screen_window\" parameter must consist of exactly two point2f values. Using default screen window.");
    }
    else
    {
        screen_window = screen_window.aroundPoints(screen_window_corners[0], screen_window_corners[1]);
    }*/

    imp_float shutter_start = parameters.getSingleFloatValue("shutter_start", 0.0f);
    imp_float shutter_end = parameters.getSingleFloatValue("shutter_end", 0.002f);
    imp_float aperture = parameters.getSingleFloatValue("aperture", 0.0f);
    imp_float focal_distance = parameters.getSingleFloatValue("focal_distance", 1.0f);
	imp_float screen_width = parameters.getSingleFloatValue("screen_width", 2.0f);
    
	imp_float aspect = sensor->full_resolution.y/static_cast<imp_float>(sensor->full_resolution.x);
	
    BoundingRectangleF screen_window(Point2F(-screen_width*0.5f, -screen_width*0.5f*aspect), Point2F(screen_width*0.5f, screen_width*0.5f*aspect));

	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Camera:"
						 "\n    %-20s%s"
						 "\n    %-20s%g s"
						 "\n    %-20s%g mm"
						 "\n    %-20s%g m"
						 "\n    %-20s%g m"
						 "\n    %-20s%s m"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Type:", (aperture == 0)? "Orthographic (pinhole)" : "Orthographic",
						 "Shutter:", shutter_end - shutter_start,
						 "Aperture:", aperture,
						 "Focal distance:", focal_distance,
						 "Screen width:", screen_width,
						 "Position:", camera_to_world(Point3F(0, 0, 0), shutter_start).toString().c_str(),
						 "Look direction:", camera_to_world(Vector3F(0, 0, -1), shutter_start).toString().c_str(),
						 "Up direction:", camera_to_world(Vector3F(0, 1, 0), shutter_start).toString().c_str());
	}

    return new OrthographicCamera(camera_to_world,
                                  screen_window,
                                  shutter_start,
                                  shutter_end,
                                  aperture*5e-4f,
                                  focal_distance,
                                  sensor,
                                  medium);
}

} // RayImpact
} // Impact
