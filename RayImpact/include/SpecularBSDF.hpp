#pragma once
#include "BSDF.hpp"
#include "FresnelReflector.hpp"

namespace Impact {
namespace RayImpact {

// SpecularBSDF declarations

class SpecularBSDF : public BXDF {

private:

    const ReflectionSpectrum reflectance; // Fraction of incident light that is reflected (disregarding Fresnel effects)
    const TransmissionSpectrum transmittance; // Fraction of incident light that is transmitted (disregarding Fresnel effects)
    const imp_float refractive_index_outside; // Index of refraction of the medium on the outside of the object
    const imp_float refractive_index_inside; // Index of refraction of the medium on the inside of the object
    const DielectricReflector dielectric_reflector; // Dielectric Fresnel reflector for the surface
    const TransportMode transport_mode;

public:

    SpecularBSDF(const ReflectionSpectrum& reflectance,
                 const TransmissionSpectrum& transmittance,
                 imp_float refractive_index_outside,
                 imp_float refractive_index_inside,
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

// SpecularBSDF inline method definitions

inline SpecularBSDF::SpecularBSDF(const ReflectionSpectrum& reflectance,
								  const TransmissionSpectrum& transmittance,
								  imp_float refractive_index_outside,
								  imp_float refractive_index_inside,
								  TransportMode transport_mode)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
      reflectance(reflectance),
      transmittance(transmittance),
      refractive_index_outside(refractive_index_outside),
      refractive_index_inside(refractive_index_inside),
      dielectric_reflector(refractive_index_outside,
                           refractive_index_inside),
      transport_mode(transport_mode)
{}

inline Spectrum SpecularBSDF::evaluate(const Vector3F& outgoing_direction,
									   const Vector3F& incident_direction) const
{
    return Spectrum(0.0f);
}

inline imp_float SpecularBSDF::pdf(const Vector3F& outgoing_direction,
								   const Vector3F& incident_direction) const
{
	return 0;
}

} // RayImpact
} // Impact
