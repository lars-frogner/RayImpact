#include "SpecularBRDF.hpp"

namespace Impact {
namespace RayImpact {

// SpecularBRDF method definitions

Spectrum SpecularBRDF::sample(const Vector3F& outgoing_direction,
                              Vector3F* incident_direction,
                              const Point2F& uniform_sample,
                              imp_float* pdf_value,
                              BXDFType* sampled_type /* = nullptr */) const
{
    *incident_direction = Vector3F(-outgoing_direction.x, -outgoing_direction.y, outgoing_direction.z);

    *pdf_value = 1.0f;

    return reflectance*fresnel_reflector->evaluate(cosTheta(*incident_direction))/absCosTheta(*incident_direction);
}

} // RayImpact
} // Impact
