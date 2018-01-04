#include "ScatteringEvent.hpp"
#include "Shape.hpp"

namespace Impact {
namespace RayImpact {

// ScatteringEvent method implementations

ScatteringEvent::ScatteringEvent()
	: position(),
	  position_error(),
	  outgoing_direction(),
	  surface_normal(),
	  medium_interface(nullptr),
	  time(0.0f)
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
	  shape(shape)
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

} // RayImpact
} // Impact
