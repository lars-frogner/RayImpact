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
                    const Point2F& uniform_sample,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

// SpecularBRDF inline method definitions

inline SpecularBRDF::SpecularBRDF(const ReflectionSpectrum& reflectance,
								  FresnelReflector* fresnel_reflector)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
      reflectance(reflectance),
      fresnel_reflector(fresnel_reflector)
{}

inline Spectrum SpecularBRDF::evaluate(const Vector3F& outgoing_direction,
									   const Vector3F& incident_direction) const
{
    return Spectrum(0.0f);
}

inline imp_float SpecularBRDF::pdf(const Vector3F& outgoing_direction,
								   const Vector3F& incident_direction) const
{
	return 0;
}

} // RayImpact
} // Impact
