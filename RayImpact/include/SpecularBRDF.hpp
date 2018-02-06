#pragma once
#include "BSDF.hpp"
#include "FresnelReflector.hpp"

namespace Impact {
namespace RayImpact {
    
// SpecularBRDF declarations

class SpecularBRDF : public BXDF {

private:

    const ReflectionSpectrum reflectance; // Fraction of incident light that is reflected (disregarding Fresnel effects)
    const FresnelReflector* fresnel_reflector; // Fresnel reflector for the surface

public:

    SpecularBRDF(const ReflectionSpectrum& reflectance,
                 FresnelReflector* fresnel_reflector);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& sample_values,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;
};

} // RayImpact
} // Impact
