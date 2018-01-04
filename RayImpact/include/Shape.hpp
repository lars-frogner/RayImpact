#pragma once
#include "precision.hpp"
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

} // RayImpact
} // Impact
