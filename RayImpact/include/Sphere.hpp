#pragma once
#include "Shape.hpp"
#include "precision.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "BoundingBox.hpp"
#include "ScatteringEvent.hpp"

namespace Impact {
namespace RayImpact {

// Sphere declarations

class Sphere : public Shape {

public:

	imp_float radius; // Radius of the sphere
	imp_float y_min; // Smallest y-coordinate of the partial sphere
	imp_float y_max; // Largest y-coordinate of the partial sphere
	imp_float theta_min; // Smallest polar angle of the partial sphere
	imp_float theta_max; // Largest polar angle of the partial sphere
	imp_float phi_max; // Largest azimuthal angle of the partial sphere

	// Theta is the angle with respect to the positive y-axis,
	// and phi is the angle in the zx-plane with respect to the
	// positive z-axis.

	Sphere(const Transformation* object_to_world,
		   const Transformation* world_to_object,
		   bool has_reverse_orientation,
		   imp_float radius,
		   imp_float y_min, imp_float y_max,
		   imp_float phi_max);

	BoundingBoxF objectSpaceBoundingBox() const;

	bool intersect(const Ray& ray,
				   imp_float* intersection_distance,
				   SurfaceScatteringEvent* scattering_event,
				   bool test_alpha_texture = true) const;

	bool hasIntersection(const Ray& ray,
						 bool test_alpha_texture = true) const;

	imp_float surfaceArea() const;
};

} // RayImpact
} // Impact
