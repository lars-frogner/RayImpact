#pragma once
#include "BSDF.hpp"

namespace Impact {
namespace RayImpact {

// LambertianBRDF declarations

class LambertianBRDF : public BXDF {

private:
    const ReflectionSpectrum reflectance; // Fraction of incident light that is reflected

public:

    LambertianBRDF(const ReflectionSpectrum& reflectance);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& uniform_sample,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    Spectrum reduced(const Vector3F& outgoing_direction,
                     unsigned int n_samples,
                     const Point2F* samples) const;

    Spectrum reduced(unsigned int n_samples,
                     const Point2F* samples_1,
                     const Point2F* samples_2) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

// LambertianBRDF inline method definitions

inline LambertianBRDF::LambertianBRDF(const ReflectionSpectrum& reflectance)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
      reflectance(reflectance)
{}

inline Spectrum LambertianBRDF::evaluate(const Vector3F& outgoing_direction,
										 const Vector3F& incident_direction) const
{
    return reflectance*IMP_ONE_OVER_PI;
}

inline Spectrum LambertianBRDF::reduced(const Vector3F& outgoing_direction,
										unsigned int n_samples,
										const Point2F* samples) const
{
    return reflectance;
}

inline Spectrum LambertianBRDF::reduced(unsigned int n_samples,
										const Point2F* samples_1,
										const Point2F* samples_2) const
{
    return reflectance;
}

inline imp_float LambertianBRDF::pdf(const Vector3F& outgoing_direction,
									 const Vector3F& incident_direction) const
{
    return sameHemisphere(outgoing_direction, incident_direction)?
		absCosTheta(incident_direction)*IMP_ONE_OVER_PI : 0;
}

} // RayImpact
} // Impact
