#include "MicrofacetBTDF.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetBTDF method implementations

MicrofacetBTDF::MicrofacetBTDF(const TransmissionSpectrum& transmittance,
                               imp_float refractive_index_outside,
                               imp_float refractive_index_inside,
                               MicrofacetDistribution* microfacet_distribution,
                               TransportMode transport_mode)
    : BXDF::BXDF(BXDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
      transmittance(transmittance),
      refractive_index_outside(refractive_index_outside),
      refractive_index_inside(refractive_index_inside),
      microfacet_distribution(microfacet_distribution),
      dielectric_reflector(refractive_index_outside,
                           refractive_index_inside),
      transport_mode(transport_mode)
{}

Spectrum MicrofacetBTDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    if (sameHemisphere(outgoing_direction, incident_direction))
        return Spectrum(0.0f);

    imp_float cos_theta_outgoing = cosTheta(outgoing_direction);
    imp_float cos_theta_incident = cosTheta(incident_direction);

    if (cos_theta_incident == 0 || cos_theta_outgoing == 0)
        return Spectrum(0.0f);

    imp_float refractive_index_ratio = (cosTheta(outgoing_direction) > 0)? refractive_index_inside/refractive_index_outside :
                                                                           refractive_index_outside/refractive_index_inside;

    Vector3F half_vector = (outgoing_direction + incident_direction*refractive_index_ratio).normalized();

    if (half_vector.z < 0)
        half_vector.reverse();

    imp_float outgoing_dot_half = outgoing_direction.dot(half_vector);
    imp_float incident_dot_half = incident_direction.dot(half_vector);

    imp_float denom_factor = outgoing_dot_half + refractive_index_ratio*incident_dot_half;

    imp_float num_factor = (transport_mode == TransportMode::Radiance)? 1.0f : refractive_index_ratio*refractive_index_ratio;

    return transmittance*
           (Spectrum(1.0f) - dielectric_reflector.evaluate(outgoing_dot_half))*
           (microfacet_distribution->areaWithMicroNormal(half_vector)*
            microfacet_distribution->visibleFraction(outgoing_direction, incident_direction)*
            num_factor*std::abs(outgoing_dot_half)*std::abs(incident_dot_half)/
            (denom_factor*denom_factor*cos_theta_incident*cos_theta_outgoing));
}

} // RayImpact
} // Impact
