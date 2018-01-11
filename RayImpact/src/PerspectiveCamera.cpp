#include "PerspectiveCamera.hpp"

namespace Impact {
namespace RayImpact {

// PerspectiveCamera method implementations

PerspectiveCamera::PerspectiveCamera(const Transformation& camera_to_world,
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
	const Point3F& origin = raster_to_camera(Point3F(0.0f, 0.0f, 0.0f));

	horizontal_pixel_offset = raster_to_camera(Point3F(1.0f, 0.0f, 0.0f)) - origin;
	vertical_pixel_offset = raster_to_camera(Point3F(0.0f, 1.0f, 0.0f)) - origin;
}

imp_float PerspectiveCamera::generateRay(const CameraSample& sample,
										 Ray* ray) const
{
	Point3F sensor_point_in_raster_space(sample.sensor_point.x, sample.sensor_point.y, 0.0f);
	
	// Find sensor point in camera space
	const Point3F& sensor_point = raster_to_camera(sensor_point_in_raster_space);
	
	// Ray starts at the origin and points towards the sensor point at the near plane in camera space
	*ray = Ray(Point3F(0.0f, 0.0f, 0.0f), Vector3F(sensor_point).normalized());

	if (lens_radius > 0.0f)
	{
		// Map [0, 1) square sample to sample on the circular lens
		const Point2F& lens_point = lens_radius*unitSquareToUnitDisk(sample.lens_point);

		// Find where the ray through the lens center intersects the plane of focus
		imp_float intersection_distance_with_plane_of_focus = focal_distance/ray->direction.z;
		const Point3F& intersection_point_with_plane_of_focus = (*ray)(intersection_distance_with_plane_of_focus);

		// Update ray to account for lens
		ray->origin = Point3F(lens_point.x, lens_point.y, 0.0f);
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
	Point3F sensor_point_in_raster_space(sample.sensor_point.x, sample.sensor_point.y, 0.0f);
	
	// Find sensor point in camera space
	const Point3F& sensor_point = raster_to_camera(sensor_point_in_raster_space);

	// Ray starts at the origin and points towards the sensor point at the near plane in camera space
	*ray = RayWithOffsets(Point3F(0.0f, 0.0f, 0.0f), Vector3F(sensor_point).normalized());

	if (lens_radius > 0.0f)
	{
		// Map [0, 1) square sample to sample on the circular lens
		const Point2F& lens_point = lens_radius*unitSquareToUnitDisk(sample.lens_point);

		// Find where the ray through the lens center intersects the plane of focus
		imp_float intersection_distance_with_plane_of_focus = focal_distance/ray->direction.z;
		Point3F intersection_point_with_plane_of_focus = (*ray)(intersection_distance_with_plane_of_focus);

		// Update ray to account for lens
		ray->origin = Point3F(lens_point.x, lens_point.y, 0.0f);
		ray->direction = (intersection_point_with_plane_of_focus - ray->origin).normalized();

		// Compute direction for x-offset ray through the lens center
		const Vector3F& x_offset_direction = Vector3F(sensor_point + horizontal_pixel_offset).normalized();

		// Compute intersection distance for the x-offset ray
		intersection_distance_with_plane_of_focus = focal_distance/x_offset_direction.z;
		
		// Compute intersection point for the x-offset ray
		intersection_point_with_plane_of_focus = Point3F(0.0f, 0.0f, 0.0f) + x_offset_direction*intersection_distance_with_plane_of_focus;
		
		ray->x_offset_ray_origin = ray->origin;
		ray->x_offset_ray_direction = (intersection_point_with_plane_of_focus - ray->x_offset_ray_origin).normalized();
	
		// Compute direction for y-offset ray through the lens center
		const Vector3F& y_offset_direction = Vector3F(sensor_point + vertical_pixel_offset).normalized();

		// Compute intersection distance for the y-offset ray
		intersection_distance_with_plane_of_focus = focal_distance/y_offset_direction.z;
		
		// Compute intersection point for the y-offset ray
		intersection_point_with_plane_of_focus = Point3F(0.0f, 0.0f, 0.0f) + y_offset_direction*intersection_distance_with_plane_of_focus;
		
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

} // RayImpact
} // Impact