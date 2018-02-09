#pragma once
#include "BSDF.hpp"

namespace Impact {
namespace RayImpact {

// LambertianBTDF declarations

class LambertianBTDF : public BXDF {

private:
    const TransmissionSpectrum transmittance; // Fraction of incident light that is transmitted

public:

    LambertianBTDF(const TransmissionSpectrum& transmittance);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& uniform_sample,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

} // RayImpact
} // Impact
