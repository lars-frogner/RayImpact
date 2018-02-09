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

} // RayImpact
} // Impact
