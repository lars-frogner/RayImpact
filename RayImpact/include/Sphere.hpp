#pragma once
#include "Shape.hpp"
#include "precision.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "BoundingBox.hpp"
#include "ScatteringEvent.hpp"
#include "ParameterSet.hpp"
#include "error.hpp"
#include <memory>

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

// Sphere function declarations

std::shared_ptr<Shape> createSphere(const Transformation* object_to_world,
                                    const Transformation* world_to_object,
                                    bool has_reverse_orientation,
                                    const ParameterSet& parameters);

// Sphere inline method definitions

inline Sphere::Sphere(const Transformation* object_to_world,
					  const Transformation* world_to_object,
					  bool has_reverse_orientation,
					  imp_float radius,
					  imp_float y_min, imp_float y_max,
					  imp_float phi_max)
    : Shape::Shape(object_to_world, world_to_object, has_reverse_orientation),
      radius(radius),
      y_min(clamp(y_min, -radius, radius)),
      y_max(clamp(y_max, -radius, radius)),
      theta_min(std::acos(clamp(y_max/radius, -1.0f, 1.0f))),
      theta_max(std::acos(clamp(y_min/radius, -1.0f, 1.0f))),
      phi_max(clamp(degreesToRadians(phi_max), 0.0f, IMP_TWO_PI))
{
    imp_assert(radius >= 0);
    imp_assert(y_max >= y_min);
}

inline imp_float Sphere::surfaceArea() const
{
    return phi_max*radius*(y_max - y_min);
}

} // RayImpact
} // Impact
