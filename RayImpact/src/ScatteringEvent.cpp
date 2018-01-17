#include "ScatteringEvent.hpp"
#include "Shape.hpp"
#include "Model.hpp"
#include "math.hpp"

namespace Impact {
namespace RayImpact {

// ScatteringEvent method implementations

ScatteringEvent::ScatteringEvent()
	: position(),
	  position_error(),
	  outgoing_direction(),
	  surface_normal(),
	  medium_interface(nullptr),
	  time(0)
{}

ScatteringEvent::ScatteringEvent(const Point3F& position,
								 const Vector3F& position_error,
							 	 const Vector3F& outgoing_direction,
								 const Normal3F& surface_normal,
								 const MediumInterface* medium_interface,
								 imp_float time)
	: position(position),
	  position_error(position_error),
	  outgoing_direction(outgoing_direction),
	  surface_normal(surface_normal),
	  medium_interface(medium_interface),
	  time(time)
{}

Ray ScatteringEvent::spawnRay(const Vector3F& direction) const
{
	const Point3F& origin = offsetRayOrigin(position, position_error, surface_normal, direction);

	return Ray(origin, direction, IMP_INFINITY, time, getMediumInDirection(direction));
}

Ray ScatteringEvent::spawnRayTo(const Point3F& end_point) const
{
	const Point3F& origin = offsetRayOrigin(position, position_error, surface_normal, end_point - position);

	const Vector3F& direction = end_point - origin;

	return Ray(origin, direction, 1.0f - IMP_SHADOW_EPS, time, getMediumInDirection(direction));
}

Ray ScatteringEvent::spawnRayTo(const ScatteringEvent& other) const
{
	return spawnRayTo(other.position);
}

bool ScatteringEvent::isOnSurface() const
{
	return surface_normal.nonZero();
}

// SurfaceScatteringEvent method implementations

SurfaceScatteringEvent::SurfaceScatteringEvent()
	: ScatteringEvent::ScatteringEvent(),
	  position_uv(),
	  position_u_deriv(),
	  position_v_deriv(),
	  normal_u_deriv(),
	  normal_v_deriv(),
	  shape(nullptr),
	  model(nullptr),
	  bsdf(nullptr),
	  bssrdf(nullptr),
	  shading()
{}

SurfaceScatteringEvent::SurfaceScatteringEvent(const Point3F& position,
											   const Vector3F& position_error,
											   const Point2F& position_uv,
											   const Vector3F& outgoing_direction,
											   const Vector3F& position_u_deriv,
											   const Vector3F& position_v_deriv,
											   const Normal3F& normal_u_deriv,
											   const Normal3F& normal_v_deriv,
											   imp_float time,
											   const Shape* shape)
	: ScatteringEvent::ScatteringEvent(position,
									   position_error,
									   outgoing_direction,
									   Normal3F(position_u_deriv.cross(position_v_deriv).normalized()),
									   nullptr,
									   time),
	  position_uv(position_uv),
	  position_u_deriv(position_u_deriv),
	  position_v_deriv(position_v_deriv),
	  normal_u_deriv(normal_u_deriv),
	  normal_v_deriv(normal_v_deriv),
	  shape(shape),
	  model(nullptr),
	  bsdf(nullptr),
	  bssrdf(nullptr)
{
	shading.surface_normal = surface_normal;
	shading.position_u_deriv = position_u_deriv;
	shading.position_v_deriv = position_v_deriv;
	shading.normal_u_deriv = normal_u_deriv;
	shading.normal_v_deriv = normal_v_deriv;

	// Reverse surface normal if either the shape is specified to
	// have reverse orientation or its transformation swaps handedness
	// (if both are true, they cancel and the normal remains unchanged)
	if (shape && (shape->has_reverse_orientation ^ shape->transformation_swaps_handedness))
	{
		surface_normal.reverse();
		shading.surface_normal.reverse();
	}
}

void SurfaceScatteringEvent::setShadingGeometry(const Vector3F& position_u_deriv,
												const Vector3F& position_v_deriv,
												const Normal3F& normal_u_deriv,
												const Normal3F& normal_v_deriv,
												bool shading_normal_determines_orientation)
{
	shading.position_u_deriv = position_u_deriv;
	shading.position_v_deriv = position_v_deriv;
	shading.normal_u_deriv = normal_u_deriv;
	shading.normal_v_deriv = normal_v_deriv;

	shading.surface_normal = Normal3F(position_u_deriv.cross(position_v_deriv).normalized());

	if (shape && (shape->has_reverse_orientation ^ shape->transformation_swaps_handedness))
	{
		shading.surface_normal.reverse();
	}

	if (shading_normal_determines_orientation)
	{
		surface_normal.flipToSameHemisphereAs(shading.surface_normal);
	}
	else
	{
		shading.surface_normal.flipToSameHemisphereAs(surface_normal);
	}
}

// Utility functions

// Returns the position of a new ray origin that is guaranteed to not result in a false
// re-intersection of the surface while still being as close to the original position as possible
Point3F offsetRayOrigin(const Point3F& ray_origin,
						const Vector3F& ray_origin_error,
						const Normal3F& surface_normal,
						const Vector3F& ray_direction)
{
	imp_float normal_offset_distance = ray_origin_error.dot(abs(surface_normal));
	Vector3F normal_offset = Vector3F(surface_normal)*normal_offset_distance;

	if (ray_direction.dot(surface_normal) < 0)
		normal_offset.reverse();

	Point3F offset_ray_origin = ray_origin + normal_offset;

	if (normal_offset.x > 0)
		offset_ray_origin.x = closestHigherFloat(offset_ray_origin.x);
	else if (normal_offset.x < 0)
		offset_ray_origin.x = closestLowerFloat(offset_ray_origin.x);

	if (normal_offset.y > 0)
		offset_ray_origin.y = closestHigherFloat(offset_ray_origin.y);
	else if (normal_offset.y < 0)
		offset_ray_origin.y = closestLowerFloat(offset_ray_origin.y);

	if (normal_offset.z > 0)
		offset_ray_origin.z = closestHigherFloat(offset_ray_origin.z);
	else if (normal_offset.z < 0)
		offset_ray_origin.z = closestLowerFloat(offset_ray_origin.z);

	return offset_ray_origin;
}

} // RayImpact
} // Impact
