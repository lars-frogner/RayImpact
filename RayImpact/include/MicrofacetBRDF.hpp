#pragma once
#include "BSDF.hpp"
#include "MicrofacetDistribution.hpp"
#include "FresnelReflector.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetBRDF declarations

class MicrofacetBRDF : public BXDF {

private:

    const ReflectionSpectrum reflectance; // Fraction of incident light that is reflected (disregarding Fresnel effects)
    const MicrofacetDistribution* microfacet_distribution; // Microfacet distribution for the surface
    const FresnelReflector* fresnel_reflector; // Fresnel reflector for the surface

public:

    MicrofacetBRDF(const ReflectionSpectrum& reflectance,
                   MicrofacetDistribution* microfacet_distribution,
                   FresnelReflector* fresnel_reflector);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;
};

} // RayImpact
} // Impact
