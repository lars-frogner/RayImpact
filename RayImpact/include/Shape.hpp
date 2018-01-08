#pragma once
#include "precision.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "BoundingBox.hpp"
#include "ScatteringEvent.hpp"

namespace Impact {
namespace RayImpact {

// Shape declarations

class Shape {

public:

	const Transformation* object_to_world; // Transformation from the object system to the world system
	const Transformation* world_to_object; // Transformation from the world system to the object system
	const bool has_reverse_orientation; // Whether surface normals should be reversed compared to the default
	const bool transformation_swaps_handedness; // Whether the object-to-world transformation swaps handedness

	Shape(const Transformation* object_to_world,
		  const Transformation* world_to_object,
		  bool has_reverse_orientation);

	virtual BoundingBoxF objectSpaceBoundingBox() const = 0;
	
	virtual BoundingBoxF worldSpaceBoundingBox() const;

	virtual bool intersect(const Ray& ray,
						   imp_float* intersection_distance,
						   SurfaceScatteringEvent* scattering_event,
						   bool test_alpha_texture = true) const = 0;

	virtual bool hasIntersection(const Ray& ray,
								 bool test_alpha_texture = true) const;

	virtual imp_float surfaceArea() const = 0;
};

// Utility function for shapes

// Computes parametric derivatives of the surface normal
inline void computeNormalDerivatives(const Vector3F& position_u_deriv,
									 const Vector3F& position_v_deriv,
									 const Vector3F& position_u2_deriv,
									 const Vector3F& position_uv_deriv,
									 const Vector3F& position_v2_deriv,
									 Normal3F* normal_u_deriv,
									 Normal3F* normal_v_deriv)
{
	// Uses the Weingarten equations

	imp_float E = position_u_deriv.squaredLength();
	imp_float F = position_u_deriv.dot(position_v_deriv);
	imp_float G = position_v_deriv.squaredLength();

	const Vector3F& surface_normal = position_u_deriv.cross(position_v_deriv).normalized();

	imp_float e = surface_normal.dot(position_u2_deriv);
	imp_float f = surface_normal.dot(position_uv_deriv);
	imp_float g = surface_normal.dot(position_v2_deriv);

	imp_float normal_deriv_norm = 1.0f/(E*G - F*F);

	*normal_u_deriv = Normal3F(position_u_deriv*((f*F - e*G)*normal_deriv_norm) +
							   position_v_deriv*((e*F - f*E)*normal_deriv_norm));

	*normal_v_deriv = Normal3F(position_u_deriv*((g*F - f*G)*normal_deriv_norm) +
							   position_v_deriv*((f*F - g*E)*normal_deriv_norm));
}

} // RayImpact
} // Impact
