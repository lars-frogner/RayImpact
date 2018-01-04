#include "Shape.hpp"

namespace Impact {
namespace RayImpact {

// Shape method implementations

Shape::Shape(const Transformation* object_to_world,
		     const Transformation* world_to_object,
		     bool has_reverse_orientation)
	: object_to_world(object_to_world),
	  world_to_object(world_to_object),
	  has_reverse_orientation(has_reverse_orientation),
	  transformation_swaps_handedness(object_to_world->swapsHandedness())
{}

BoundingBoxF Shape::worldSpaceBoundingBox() const
{
	return (*object_to_world)(objectSpaceBoundingBox());
}

bool Shape::hasIntersection(const Ray& ray,
							bool test_alpha_texture /* = true */) const
{
	imp_float intersection_distance = ray.max_distance;
	SurfaceScatteringEvent scattering_event;

	return intersect(ray, &intersection_distance, &scattering_event, test_alpha_texture);
}

} // RayImpact
} // Impact
