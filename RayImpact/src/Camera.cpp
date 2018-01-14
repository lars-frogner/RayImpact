#include "Camera.hpp"

namespace Impact {
namespace RayImpact {

// Camera method implementations

Camera::Camera(const Transformation& camera_to_world,
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

imp_float Camera::generateRayWithOffsets(const CameraSample& sample,
										 RayWithOffsets* ray) const
{
	imp_float ray_weight = generateRay(sample, ray);

	 // Offset sensor point by one pixel in the x-direction
	CameraSample x_offset_sample = sample;
	x_offset_sample.sensor_point.x += 1.0f;

	// Generate x-offset ray
	Ray x_offset_ray;
	imp_float x_offset_ray_weight = generateRay(x_offset_sample, &x_offset_ray);

	if (x_offset_ray_weight == 0.0f)
		return 0.0f;

	ray->x_offset_ray_origin = x_offset_ray.origin;
	ray->x_offset_ray_direction = x_offset_ray.direction;
	
	 // Offset sensor point by one pixel in the y-direction
	CameraSample y_offset_sample = sample;
	y_offset_sample.sensor_point.y += 1.0f;

	// Generate y-offset ray
	Ray y_offset_ray;
	imp_float y_offset_ray_weight = generateRay(y_offset_sample, &y_offset_ray);

	if (y_offset_ray_weight == 0.0f)
		return 0.0f;

	ray->y_offset_ray_origin = y_offset_ray.origin;
	ray->y_offset_ray_direction = y_offset_ray.direction;

	ray->has_offsets = true;

	return ray_weight;
}

// ProjectiveCamera method implementations

ProjectiveCamera::ProjectiveCamera(const Transformation& camera_to_world,
								   const Transformation& camera_to_screen,
								   const BoundingRectangleF& screen_window,
								   imp_float shutter_opening_time,
								   imp_float shutter_closing_time,
								   imp_float lens_radius,
								   imp_float focal_distance,
								   Sensor* sensor,
								   const Medium* medium)
	: Camera::Camera(camera_to_world,
					 shutter_opening_time,
					 shutter_closing_time,
					 sensor,
					 medium),
	  camera_to_screen(camera_to_screen),
	  lens_radius(lens_radius),
	  focal_distance(focal_distance)
{
	screen_to_raster = Transformation::scaling((imp_float)(sensor->full_resolution.x),
											   (imp_float)(sensor->full_resolution.y),
											   1.0f)*
					   Transformation::scaling( 1.0f/(screen_window.upper_corner.x - screen_window.lower_corner.x),
											   -1.0f/(screen_window.upper_corner.y - screen_window.lower_corner.y),
											    1.0f)*
					   Transformation::translation(Vector3F(-screen_window.lower_corner.x,
															-screen_window.upper_corner.y,
															0.0f));

	raster_to_screen = screen_to_raster.inverted();

	raster_to_camera = camera_to_screen.inverted()*raster_to_screen;
}

} // RayImpact
} // Impact
