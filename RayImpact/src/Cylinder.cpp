#include "Cylinder.hpp"
#include "math.hpp"
#include "ErrorFloat.hpp"
#include "Point3.hpp"
#include <cassert>
#include <cmath>

namespace Impact {
namespace RayImpact {

// Cylinder method implementations

Cylinder::Cylinder(const Transformation* object_to_world,
		 		   const Transformation* world_to_object,
				   bool has_reverse_orientation,
				   imp_float radius,
				   imp_float y_min, imp_float y_max,
				   imp_float phi_max)
	: Shape::Shape(object_to_world, world_to_object, has_reverse_orientation),
	  radius(radius),
	  y_min(y_min),
	  y_max(y_max),
	  phi_max(clamp(degreesToRadians(phi_max), 0.0f, IMP_TWO_PI))
{
	assert(radius >= 0);
	assert(y_max >= y_min);
}

BoundingBoxF Cylinder::objectSpaceBoundingBox() const
{
	if (phi_max >= IMP_THREE_PI_OVER_TWO)
	{
		return BoundingBoxF(Point3F(-radius, y_min, -radius),
							Point3F( radius, y_max,  radius));
	}
	else if (phi_max >= IMP_PI)
	{
		return BoundingBoxF(Point3F(std::sin(phi_max)*radius, y_min, -radius),
							Point3F(			      radius, y_max,  radius));
	}
	else if (phi_max >= IMP_PI_OVER_TWO)
	{
		return BoundingBoxF(Point3F(					0.0f, y_min, std::cos(phi_max)*radius),
							Point3F(std::sin(phi_max)*radius, y_max,				   radius));
	}
	else
	{
		return BoundingBoxF(Point3F(					0.0f, y_min,					 0.0f),
							Point3F(std::sin(phi_max)*radius, y_max, std::cos(phi_max)*radius));
	}
}

bool Cylinder::intersect(const Ray& ray,
					     imp_float* intersection_distance,
					     SurfaceScatteringEvent* scattering_event,
					     bool test_alpha_texture /* = true */) const
{
	// Transform ray to object space

	Vector3F transformed_ray_origin_error;
	Vector3F transformed_ray_direction_error;

	const Ray& transformed_ray = (*world_to_object)(ray, &transformed_ray_origin_error, &transformed_ray_direction_error);

	// Initialize copies of ray origin and direction with associated errors

	ErrorFloat origin_x(transformed_ray.origin.x, transformed_ray_origin_error.x);
	ErrorFloat origin_y(transformed_ray.origin.y, transformed_ray_origin_error.y);
	ErrorFloat origin_z(transformed_ray.origin.z, transformed_ray_origin_error.z);
	
	ErrorFloat direction_x(transformed_ray.direction.x, transformed_ray_direction_error.x);
	ErrorFloat direction_y(transformed_ray.direction.y, transformed_ray_direction_error.y);
	ErrorFloat direction_z(transformed_ray.direction.z, transformed_ray_direction_error.z);

	// Compute coefficients of the quadratic equation for the intersection distance
	const ErrorFloat& a = direction_x*direction_x + direction_z*direction_z;
	const ErrorFloat& b = 2.0f*(direction_x*origin_x + direction_z*origin_z);
	const ErrorFloat& c = origin_x*origin_x + origin_z*origin_z - ErrorFloat(radius)*ErrorFloat(radius);

	// Solve quadratic equation for intersection distance

	ErrorFloat shortest_intersect_dist; // Shortest tentative intersection distance
	ErrorFloat longest_intersect_dist; // Longest tentative intersection distance

	// The ray misses if there is no solution
	if (!solveQuadraticEquation(a, b, c, &shortest_intersect_dist, &longest_intersect_dist))
		return false;

	// Find relevant intersection distance

	// The ray misses if the intersection distances are outside the range (0, max_distance)
	if (shortest_intersect_dist.upperBound() > transformed_ray.max_distance ||
		longest_intersect_dist.lowerBound() < 0.0f)
		return false;
	
	ErrorFloat relevant_intersect_dist = shortest_intersect_dist;

	if (relevant_intersect_dist.lowerBound() <= 0.0f)
	{
		// Try the longest intersection distance if the ray origin is inside the cylinder
		relevant_intersect_dist = longest_intersect_dist;

		if (relevant_intersect_dist.upperBound() > transformed_ray.max_distance)
		{
			return false;
		}
	}

	// Compute intersection point
	Point3F intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));

	// Project the intersection point onto the cylinder (required in case the computed result has inaccuracies)
	imp_float inverse_intersection_radius = 1.0f/std::sqrt(intersection_point.x*intersection_point.x + intersection_point.z*intersection_point.z);
	intersection_point.x *= radius*inverse_intersection_radius;
	intersection_point.z *= radius*inverse_intersection_radius;

	// Compute azimuthal angle of intersection point
	imp_float intersection_phi = std::atan2(intersection_point.x, intersection_point.z);
	
	// Remap from [-pi, pi] to [0, 2*pi]
	if (intersection_phi < 0.0f)
		intersection_phi += IMP_TWO_PI;

	// Check whether the intersection point is outside the parameter bounds
	if (intersection_point.y < y_min ||
		intersection_point.y > y_max ||
		intersection_phi > phi_max)
	{
		// If the intersection point was the furthest one, the ray misses the partial cylinder
		if (relevant_intersect_dist == longest_intersect_dist)
			return false;

		// At this point we know that the closest intersection point does not lie
		// on the partial cylinder, but the furthest intersection point still might

		// If the longest intersection distance is too far away, the ray misses
		if (longest_intersect_dist.upperBound() > transformed_ray.max_distance)
			return false;

		// We now have to try again with the furthest intersection point

		relevant_intersect_dist = longest_intersect_dist;
		
		intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));
		
		inverse_intersection_radius = 1.0f/std::sqrt(intersection_point.x*intersection_point.x + intersection_point.z*intersection_point.z);
		intersection_point.x *= radius*inverse_intersection_radius;
		intersection_point.z *= radius*inverse_intersection_radius;

		intersection_phi = std::atan2(intersection_point.x, intersection_point.z);
	
		if (intersection_phi < 0.0f)
			intersection_phi += IMP_TWO_PI;

		if (intersection_point.y < y_min ||
			intersection_point.y > y_max ||
			intersection_phi > phi_max)
		{
			return false;
		}
	}

	// Compute u and v coordinates of the intersection point
	
	imp_float y_range = y_max - y_min;

	imp_float u = intersection_phi/phi_max;
	imp_float v = (intersection_point.y - y_min)/y_range;

	// Compute u and v derivatives of the intersection point

	const Vector3F& position_u_deriv = Vector3F(-intersection_point.z*phi_max, 0.0f, intersection_point.x*phi_max);

	const Vector3F& position_v_deriv = Vector3F(0.0f, y_range, 0.0f);

	// Compute u and v derivatives of the surface normal

	const Vector3F& position_u2_deriv = Vector3F(intersection_point.x, 0.0f, intersection_point.z)*(-phi_max*phi_max);
	//				position_uv_deriv = (0, 0, 0)
	//				position_v2_deriv = (0, 0, 0)

	imp_float E = position_u_deriv.squaredLength();
	//		  F = 0
	//		  G = y_range^2;

	const Vector3F& surface_normal = position_u_deriv.cross(position_v_deriv).normalized();

	imp_float e = surface_normal.dot(position_u2_deriv);
	//		  f = 0
	//		  g = 0

	const Normal3F& normal_u_deriv = Normal3F(position_u_deriv*(-e/E));
	//				normal_v_deriv = (0, 0, 0)

	// Compute error for intersection point
	const Vector3F& intersection_point_error = Vector3F(std::abs(intersection_point.x), 0.0f, std::abs(intersection_point.z))*errorPowerBound(3);

	// Construct scattering event
	*scattering_event = (*object_to_world)(SurfaceScatteringEvent(intersection_point,
																  intersection_point_error,
																  Point2F(u, v),
																  -transformed_ray.direction,
																  position_u_deriv,
																  position_v_deriv,
																  normal_u_deriv,
																  Normal3F(),
																  transformed_ray.time,
																  this));

	// Set intersection distance (is correct for world space even though it was computed for object space)
	*intersection_distance = static_cast<imp_float>(relevant_intersect_dist);

	return true;
}

bool Cylinder::hasIntersection(const Ray& ray,
							   bool test_alpha_texture /* = true */) const
{
	// Transform ray to object space

	Vector3F transformed_ray_origin_error;
	Vector3F transformed_ray_direction_error;

	const Ray& transformed_ray = (*world_to_object)(ray, &transformed_ray_origin_error, &transformed_ray_direction_error);

	// Initialize copies of ray origin and direction with associated errors

	ErrorFloat origin_x(transformed_ray.origin.x, transformed_ray_origin_error.x);
	ErrorFloat origin_y(transformed_ray.origin.y, transformed_ray_origin_error.y);
	ErrorFloat origin_z(transformed_ray.origin.z, transformed_ray_origin_error.z);
	
	ErrorFloat direction_x(transformed_ray.direction.x, transformed_ray_direction_error.x);
	ErrorFloat direction_y(transformed_ray.direction.y, transformed_ray_direction_error.y);
	ErrorFloat direction_z(transformed_ray.direction.z, transformed_ray_direction_error.z);

	// Compute coefficients of the quadratic equation for the intersection distance
	const ErrorFloat& a = direction_x*direction_x + direction_z*direction_z;
	const ErrorFloat& b = 2.0f*(direction_x*origin_x + direction_z*origin_z);
	const ErrorFloat& c = origin_x*origin_x + origin_z*origin_z - ErrorFloat(radius)*ErrorFloat(radius);

	// Solve quadratic equation for intersection distance

	ErrorFloat shortest_intersect_dist; // Shortest tentative intersection distance
	ErrorFloat longest_intersect_dist; // Longest tentative intersection distance

	// The ray misses if there is no solution
	if (!solveQuadraticEquation(a, b, c, &shortest_intersect_dist, &longest_intersect_dist))
		return false;

	// Find relevant intersection distance

	// The ray misses if the intersection distances are outside the range (0, max_distance)
	if (shortest_intersect_dist.upperBound() > transformed_ray.max_distance ||
		longest_intersect_dist.lowerBound() < 0.0f)
		return false;
	
	ErrorFloat relevant_intersect_dist = shortest_intersect_dist;

	if (relevant_intersect_dist.lowerBound() <= 0.0f)
	{
		// Try the longest intersection distance if the ray origin is inside the cylinder
		relevant_intersect_dist = longest_intersect_dist;

		if (relevant_intersect_dist.upperBound() > transformed_ray.max_distance)
		{
			return false;
		}
	}

	// Compute intersection point
	Point3F intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));

	// Project the intersection point onto the cylinder (required in case the computed result has inaccuracies)
	imp_float inverse_intersection_radius = 1.0f/std::sqrt(intersection_point.x*intersection_point.x + intersection_point.z*intersection_point.z);
	intersection_point.x *= radius*inverse_intersection_radius;
	intersection_point.z *= radius*inverse_intersection_radius;

	imp_float intersection_phi;

	if (phi_max < IMP_TWO_PI)
	{
		// Compute azimuthal angle of intersection point
		intersection_phi = std::atan2(intersection_point.x, intersection_point.z);
	
		// Remap from [-pi, pi] to [0, 2*pi]
		if (intersection_phi < 0.0f)
			intersection_phi += IMP_TWO_PI;
	}
	else
	{
		intersection_phi = IMP_TWO_PI;
	}

	// Check whether the intersection point is outside the parameter bounds
	if (intersection_point.y < y_min ||
		intersection_point.y > y_max ||
		intersection_phi > phi_max)
	{
		// If the intersection point was the furthest one, the ray misses the partial cylinder
		if (relevant_intersect_dist == longest_intersect_dist)
			return false;

		// At this point we know that the closest intersection point does not lie
		// on the partial cylinder, but the furthest intersection point still might

		// If the longest intersection distance is too far away, the ray misses
		if (longest_intersect_dist.upperBound() > transformed_ray.max_distance)
			return false;

		// We now have to try again with the furthest intersection point

		relevant_intersect_dist = longest_intersect_dist;
		
		intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));

		inverse_intersection_radius = 1.0f/std::sqrt(intersection_point.x*intersection_point.x + intersection_point.z*intersection_point.z);
		intersection_point.x *= radius*inverse_intersection_radius;
		intersection_point.z *= radius*inverse_intersection_radius;

		if (phi_max < IMP_TWO_PI)
		{
			// Compute azimuthal angle of intersection point
			intersection_phi = std::atan2(intersection_point.x, intersection_point.z);
	
			// Remap from [-pi, pi] to [0, 2*pi]
			if (intersection_phi < 0.0f)
				intersection_phi += IMP_TWO_PI;
		}
		else
		{
			return false;
		}

		if (intersection_point.y < y_min ||
			intersection_point.y > y_max ||
			intersection_phi > phi_max)
		{
			return false;
		}
	}

	return true;
}

imp_float Cylinder::surfaceArea() const
{
	return phi_max*radius*(y_max - y_min);
}

} // RayImpact
} // Impact
