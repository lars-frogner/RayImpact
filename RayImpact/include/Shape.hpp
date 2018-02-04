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

// Computes derivatives of the surface normal with respect to the surface parameters
inline void computeNormalDerivatives(const Vector3F& dpdu,
                                     const Vector3F& dpdv,
                                     const Vector3F& d2pdu2,
                                     const Vector3F& d2pdudv,
                                     const Vector3F& d2pdv2,
                                     Normal3F* dndu,
                                     Normal3F* dndv)
{
    // Uses the Weingarten equations

    imp_float E = dpdu.squaredLength();
    imp_float F = dpdu.dot(dpdv);
    imp_float G = dpdv.squaredLength();

    const Vector3F& surface_normal = dpdu.cross(dpdv).normalized();

    imp_float e = surface_normal.dot(d2pdu2);
    imp_float f = surface_normal.dot(d2pdudv);
    imp_float g = surface_normal.dot(d2pdv2);

    imp_float normal_deriv_norm = 1.0f/(E*G - F*F);

    *dndu = Normal3F(dpdu*((f*F - e*G)*normal_deriv_norm) +
                     dpdv*((e*F - f*E)*normal_deriv_norm));

    *dndv = Normal3F(dpdu*((g*F - f*G)*normal_deriv_norm) +
                     dpdv*((f*F - g*E)*normal_deriv_norm));
}

} // RayImpact
} // Impact
