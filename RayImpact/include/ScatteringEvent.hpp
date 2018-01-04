#pragma once
#include "precision.hpp"
#include "Vector3.hpp"
#include "Point3.hpp"
#include "Normal3.hpp"
#include "Point2.hpp"

namespace Impact {
namespace RayImpact {

// Forward declarations

class Shape;

class MediumInterface;

// ScatteringEvent declarations

class ScatteringEvent {

public:
	Point3F position; // Location of scattering event
	Vector3F position_error; // Error in scattering position
	Vector3F outgoing_direction; // Direction of the photon after scattering
	Normal3F surface_normal; // Normal vector for the surface (if present) at the scattering position
	const MediumInterface* medium_interface;
	imp_float time; // Point in time when the scattering event happens

	ScatteringEvent();

	ScatteringEvent(const Point3F& position,
					const Vector3F& position_error,
					const Vector3F& outgoing_direction,
					const Normal3F& surface_normal,
					const MediumInterface* medium_interface,
					imp_float time);

	bool isOnSurface() const;
};

// SurfaceScatteringEvent declarations

class SurfaceScatteringEvent : public ScatteringEvent {

public:

	Point2F position_uv; // Scattering position in terms of surface parameters u and v
	Vector3F position_u_deriv; // Derivative of position with respect to u
	Vector3F position_v_deriv; // Derivative of position with respect to v
	Normal3F normal_u_deriv; // Derivative of surface normal with respect to u
	Normal3F normal_v_deriv; // Derivative of surface normal with respect to v
	const Shape* shape; // Shape representing the surface

	// Versions of surface normal and derivatives for lighting calculations
	struct 
	{
		Normal3F surface_normal;
		Vector3F position_u_deriv;
		Vector3F position_v_deriv;
		Normal3F normal_u_deriv;
		Normal3F normal_v_deriv;

	} shading;

	SurfaceScatteringEvent();

	SurfaceScatteringEvent(const Point3F& position,
						   const Vector3F& position_error,
						   const Point2F& position_uv,
						   const Vector3F& outgoing_direction,
						   const Vector3F& position_u_deriv,
						   const Vector3F& position_v_deriv,
						   const Normal3F& normal_u_deriv,
						   const Normal3F& normal_v_deriv,
						   imp_float time,
						   const Shape* shape);

	void setShadingGeometry(const Vector3F& position_u_deriv,
						    const Vector3F& position_v_deriv,
						    const Normal3F& normal_u_deriv,
						    const Normal3F& normal_v_deriv,
							bool shading_normal_determines_orientation);
};

} // RayImpact
} // Impact
