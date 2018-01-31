#pragma once
#include "Shape.hpp"
#include "precision.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "BoundingBox.hpp"
#include "ScatteringEvent.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// Disk declarations

class Disk : public Shape {

public:

	imp_float radius; // Outer radius of the disk
	imp_float inner_radius; // Inner radius of the disk
	imp_float y; // y-coordinate of the disk plane
	imp_float phi_max; // Largest azimuthal angle of the partial disk

	// Phi is the angle in the zx-plane with respect to the positive z-axis.

	Disk(const Transformation* object_to_world,
		 const Transformation* world_to_object,
	     bool has_reverse_orientation,
	     imp_float radius, imp_float inner_radius,
	     imp_float y,
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

// Disk creation

std::shared_ptr<Shape> createDisk(const Transformation* object_to_world,
								  const Transformation* world_to_object,
								  bool has_reverse_orientation,
								  const ParameterSet& parameters);

} // RayImpact
} // Impact
