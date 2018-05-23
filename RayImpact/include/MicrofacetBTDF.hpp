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

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& uniform_sample,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

// MicrofacetBTDF inline method definitions

inline MicrofacetBTDF::MicrofacetBTDF(const TransmissionSpectrum& transmittance,
									  imp_float refractive_index_outside,
									  imp_float refractive_index_inside,
									  MicrofacetDistribution* microfacet_distribution,
									  TransportMode transport_mode)
    : BXDF::BXDF(BXDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
      transmittance(transmittance),
      refractive_index_outside(refractive_index_outside),
      refractive_index_inside(refractive_index_inside),
      microfacet_distribution(microfacet_distribution),
      dielectric_reflector(refractive_index_outside,
                           refractive_index_inside),
      transport_mode(transport_mode)
{}

} // RayImpact
} // Impact
