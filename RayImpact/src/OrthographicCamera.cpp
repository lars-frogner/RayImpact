#include "OrthographicCamera.hpp"

namespace Impact {
namespace RayImpact {

// OrthographicCamera method implementations

OrthographicCamera::OrthographicCamera(const Transformation& camera_to_world,
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
		const Point2F& lens_point = lens_radius*unitSquareToUnitDisk(sample.lens_point);

		// Find where the ray through the lens center intersects the plane of focus
		imp_float intersection_distance_with_plane_of_focus = focal_distance/ray->direction.z;
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
		const Point2F& lens_point = lens_radius*unitSquareToUnitDisk(sample.lens_point);

		// Find where the ray through the lens center intersects the plane of focus
		imp_float intersection_distance_with_plane_of_focus = focal_distance/ray->direction.z;
		Point3F intersection_point_with_plane_of_focus = (*ray)(intersection_distance_with_plane_of_focus);

		// Update ray to account for lens
		ray->origin = Point3F(lens_point.x, lens_point.y, 0);
		ray->direction = (intersection_point_with_plane_of_focus - ray->origin).normalized();
		
		// Compute new intersection distance with the new ray
		intersection_distance_with_plane_of_focus = focal_distance/ray->direction.z;

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

} // RayImpact
} // Impact
