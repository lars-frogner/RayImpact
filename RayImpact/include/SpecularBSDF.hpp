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
                    const Point2F& sample_values,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;
};

} // RayImpact
} // Impact
