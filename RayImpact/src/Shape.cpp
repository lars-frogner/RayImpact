#include "Shape.hpp"

namespace Impact {
namespace RayImpact {

// Shape method definitions

bool Shape::hasIntersection(const Ray& ray,
                            bool test_alpha_texture /* = true */) const
{
    imp_float intersection_distance = ray.max_distance;
    SurfaceScatteringEvent scattering_event;

    return intersect(ray, &intersection_distance, &scattering_event, test_alpha_texture);
}

// Shape function definitions

// Computes derivatives of the surface normal with respect to the surface parameters
void computeNormalDerivatives(const Vector3F& dpdu,
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
