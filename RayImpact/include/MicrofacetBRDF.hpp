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

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& uniform_sample,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

// MicrofacetBRDF inline method definitions

inline MicrofacetBRDF::MicrofacetBRDF(const ReflectionSpectrum& reflectance,
									  MicrofacetDistribution* microfacet_distribution,
									  FresnelReflector* fresnel_reflector)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
      reflectance(reflectance),
      microfacet_distribution(microfacet_distribution),
      fresnel_reflector(fresnel_reflector)
{}

} // RayImpact
} // Impact
