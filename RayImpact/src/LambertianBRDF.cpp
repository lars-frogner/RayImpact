#include "LambertianBRDF.hpp"
#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// LambertianBRDF method implementations

LambertianBRDF::LambertianBRDF(const ReflectionSpectrum& reflectance)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
      reflectance(reflectance)
{}

Spectrum LambertianBRDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    return reflectance*IMP_ONE_OVER_PI;
}

Spectrum LambertianBRDF::sample(const Vector3F& outgoing_direction,
                                Vector3F* incident_direction,
                                const Point2F& uniform_sample,
                                imp_float* pdf_value,
                                BXDFType* sampled_type /* = nullptr */) const
{
    *incident_direction = cosineWeightedHemisphereSample(uniform_sample);

    if (outgoing_direction.z < 0)
        incident_direction->z = -incident_direction->z;

    *pdf_value = pdf(outgoing_direction, *incident_direction);

    return evaluate(outgoing_direction, *incident_direction);
}

Spectrum LambertianBRDF::reduced(const Vector3F& outgoing_direction,
                                 unsigned int n_samples,
                                 const Point2F* samples) const
{
    return reflectance;
}

Spectrum LambertianBRDF::reduced(unsigned int n_samples,
                                 const Point2F* samples_1,
                                 const Point2F* samples_2) const
{
    return reflectance;
}

imp_float LambertianBRDF::pdf(const Vector3F& outgoing_direction,
                              const Vector3F& incident_direction) const
{
    return (sameHemisphere(outgoing_direction, incident_direction))? absCosTheta(incident_direction)*IMP_ONE_OVER_PI : 0;
}

} // RayImpact
} // Impact
