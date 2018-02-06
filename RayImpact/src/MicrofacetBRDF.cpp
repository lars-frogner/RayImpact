#include "MicrofacetBRDF.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetBRDF method implementations

MicrofacetBRDF::MicrofacetBRDF(const ReflectionSpectrum& reflectance,
                               MicrofacetDistribution* microfacet_distribution,
                               FresnelReflector* fresnel_reflector)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
      reflectance(reflectance),
      microfacet_distribution(microfacet_distribution),
      fresnel_reflector(fresnel_reflector)
{}

Spectrum MicrofacetBRDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    imp_float cos_theta_outgoing = absCosTheta(outgoing_direction);
    imp_float cos_theta_incident = absCosTheta(incident_direction);

    Vector3F half_vector = outgoing_direction + incident_direction;

    if (cos_theta_incident == 0 || cos_theta_outgoing == 0 || !half_vector.nonZero())
        return Spectrum(0.0f);

    half_vector.normalize();

    return reflectance*
           fresnel_reflector->evaluate(outgoing_direction.dot(half_vector))*
           (microfacet_distribution->areaWithMicroNormal(half_vector)*
            microfacet_distribution->visibleFraction(outgoing_direction, incident_direction)/
            (4*cos_theta_incident*cos_theta_outgoing));
}

} // RayImpact
} // Impact
