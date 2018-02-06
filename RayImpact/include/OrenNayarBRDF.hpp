#pragma once
#include "BSDF.hpp"

namespace Impact {
namespace RayImpact {

// OrenNayarBRDF declarations

class OrenNayarBRDF : public BXDF {

private:

    const ReflectionSpectrum reflectance; // Fraction of incident light that is reflected
    imp_float A, B; // Coefficients for the BRSD evaluation

public:

    OrenNayarBRDF(const ReflectionSpectrum& reflectance,
                  imp_float slope_deviation);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;
};

} // RayImpact
} // Impact
