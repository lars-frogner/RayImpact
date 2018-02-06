#include "SpecularBSDF.hpp"

namespace Impact {
namespace RayImpact {

// SpecularBSDF method implementations

SpecularBSDF::SpecularBSDF(const ReflectionSpectrum& reflectance,
                           const TransmissionSpectrum& transmittance,
                           imp_float refractive_index_outside,
                           imp_float refractive_index_inside,
                           TransportMode transport_mode)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
      reflectance(reflectance),
      transmittance(transmittance),
      refractive_index_outside(refractive_index_outside),
      refractive_index_inside(refractive_index_inside),
      dielectric_reflector(refractive_index_outside,
                           refractive_index_inside),
      transport_mode(transport_mode)
{}

Spectrum SpecularBSDF::evaluate(const Vector3F& outgoing_direction,
                                const Vector3F& incident_direction) const
{
    return Spectrum(0.0f);
}

Spectrum SpecularBSDF::sample(const Vector3F& outgoing_direction,
                              Vector3F* incident_direction,
                              const Point2F& sample_values,
                              imp_float* pdf_value,
                              BXDFType* sampled_type /* = nullptr */) const
{
    printSevereMessage("SpecularBSDF::sample is not implemented");
    return Spectrum(0.0f);
}

} // RayImpact
} // Impact
