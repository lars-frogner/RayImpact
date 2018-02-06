#include "LambertianBTDF.hpp"
#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// LambertianBTDF method implementations

LambertianBTDF::LambertianBTDF(const TransmissionSpectrum& transmittance)
    : BXDF::BXDF(BXDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)),
      transmittance(transmittance)
{}

Spectrum LambertianBTDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    return transmittance*IMP_ONE_OVER_PI;
}

Spectrum LambertianBTDF::sample(const Vector3F& outgoing_direction,
                                Vector3F* incident_direction,
                                const Point2F& sample_values,
                                imp_float* pdf_value,
                                BXDFType* sampled_type /* = nullptr */) const
{
    *incident_direction = cosineWeightedHemisphereSample(sample_values);

    if (outgoing_direction.z > 0)
        incident_direction->z = -incident_direction->z;

    *pdf_value = pdf(outgoing_direction, *incident_direction);

    return evaluate(outgoing_direction, *incident_direction);
}

imp_float LambertianBTDF::pdf(const Vector3F& outgoing_direction,
                              const Vector3F& incident_direction) const
{
    return (!sameHemisphere(outgoing_direction, incident_direction))? absCosTheta(incident_direction)*IMP_ONE_OVER_PI : 0;
}

} // RayImpact
} // Impact
