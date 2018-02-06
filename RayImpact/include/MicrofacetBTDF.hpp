#pragma once
#include "BSDF.hpp"
#include "MicrofacetDistribution.hpp"
#include "FresnelReflector.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetBTDF declarations

class MicrofacetBTDF : public BXDF {

private:

    const TransmissionSpectrum transmittance; // Fraction of incident light that is transmitted (disregarding Fresnel effects)
    const imp_float refractive_index_outside; // Index of refraction of the medium on the outside of the object
    const imp_float refractive_index_inside; // Index of refraction of the medium on the inside of the object
    const MicrofacetDistribution* microfacet_distribution; // Microfacet distribution for the surface
    const DielectricReflector dielectric_reflector; // Dielectric Fresnel reflector for the surface
    const TransportMode transport_mode;

public:

    MicrofacetBTDF(const TransmissionSpectrum& transmittance,
                   imp_float refractive_index_outside,
                   imp_float refractive_index_inside,
                   MicrofacetDistribution* microfacet_distribution,
                   TransportMode transport_mode);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;
};

} // RayImpact
} // Impact
