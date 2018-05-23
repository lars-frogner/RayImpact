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

// Cylinder declarations

class Cylinder : public Shape {

public:

    imp_float radius; // Radius of the cylinder
    imp_float y_min; // y-coordinate for the bottom of the cylinder
    imp_float y_max; // y-coordinate for the top of the cylinder
    imp_float phi_max; // Largest azimuthal angle of the partial cylinder

    // Phi is the angle in the zx-plane with respect to the positive z-axis.

    Cylinder(const Transformation* object_to_world,
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

// Cylinder function declarations

std::shared_ptr<Shape> createCylinder(const Transformation* object_to_world,
                                      const Transformation* world_to_object,
                                      bool has_reverse_orientation,
                                      const ParameterSet& parameters);

// Cylinder inline method definitions

inline Cylinder::Cylinder(const Transformation* object_to_world,
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
    imp_assert(radius >= 0);
    imp_assert(y_max >= y_min);
}

inline imp_float Cylinder::surfaceArea() const
{
    return phi_max*radius*(y_max - y_min);
}

} // RayImpact
} // Impact
