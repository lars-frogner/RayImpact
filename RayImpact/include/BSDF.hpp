#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include "math.hpp"
#include "Spectrum.hpp"
#include "MicrofacetDistribution.hpp"
#include "ScatteringEvent.hpp"
#include <cmath>
#include <algorithm>

namespace Impact {
namespace RayImpact {

// BSDF utility functions

imp_float fresnelReflectance(imp_float cos_incident_angle,
                             imp_float refractive_index_outside,
                             imp_float refractive_index_inside);

ReflectionSpectrum fresnelReflectance(imp_float cos_incident_angle,
                                      const Spectrum& refractive_index_outside,
                                      const Spectrum& refractive_index_inside,
                                      const Spectrum& absorption_coefficient_inside);

bool refract(const Vector3F& incident_direction,
             const Normal3F& incident_surface_normal,
             imp_float refractive_index_incident_medium,
             imp_float refractive_index_transmitted_medium,
             Vector3F* transmitted_direction);

// FresnelReflector declarations

class FresnelReflector {

public:
    virtual ReflectionSpectrum evaluate(imp_float cos_incident_angle) const = 0;
};

// PerfectReflector implementation

class PerfectReflector : public FresnelReflector {

public:

    ReflectionSpectrum evaluate(imp_float cos_incident_angle) const
    {
        return ReflectionSpectrum(1.0f);
    }
};

// DielectricReflector implementation

class DielectricReflector : public FresnelReflector {

private:
    imp_float refractive_index_outside; // Index of refraction of the medium on the outside of the object
    imp_float refractive_index_inside; // Index of refraction of the medium on the inside of the object

public:

    DielectricReflector(imp_float refractive_index_outside,
                        imp_float refractive_index_inside)
        : refractive_index_outside(refractive_index_outside),
          refractive_index_inside(refractive_index_inside)
    {}

    ReflectionSpectrum evaluate(imp_float cos_incident_angle) const
    {
        return ReflectionSpectrum(fresnelReflectance(cos_incident_angle,
                                                     refractive_index_outside,
                                                     refractive_index_inside));
    }
};

// ConductiveReflector implementation

class ConductiveReflector : public FresnelReflector {

private:
    Spectrum refractive_index_outside; // Index of refraction of the medium on the outside of the object
    Spectrum refractive_index_inside; // Index of refraction of the medium on the inside of the object
    Spectrum absorption_coefficient_inside; // Absorption coefficient of the conductive medium on the inside of the object

public:

    ConductiveReflector(const Spectrum& refractive_index_outside,
                        const Spectrum& refractive_index_inside,
                        const Spectrum& absorption_coefficient_inside)
        : refractive_index_outside(refractive_index_outside),
          refractive_index_inside(refractive_index_inside),
          absorption_coefficient_inside(absorption_coefficient_inside)
    {}

    ReflectionSpectrum operator()(imp_float cos_incident_angle) const
    {
        return fresnelReflectance(std::abs(cos_incident_angle),
                                  refractive_index_outside,
                                  refractive_index_inside,
                                  absorption_coefficient_inside);
    }
};

// BXDF declarations

// Flags for identifying the type of BRDF/BTDF
enum BXDFType
{
    BSDF_REFLECTION   = 1 << 0,
    BSDF_TRANSMISSION = 1 << 1,
    BSDF_DIFFUSE      = 1 << 2,
    BSDF_GLOSSY       = 1 << 3,
    BSDF_SPECULAR     = 1 << 4,
    BSDF_ALL          = BSDF_REFLECTION   |
                        BSDF_TRANSMISSION |
                        BSDF_DIFFUSE      |
                        BSDF_GLOSSY       |
                        BSDF_SPECULAR
};

class BXDF {

public:

    const BXDFType type;

    BXDF(BXDFType type);

    bool containedIn(BXDFType t) const;
    bool contains(BXDFType t) const;

    virtual Spectrum evaluate(const Vector3F& outgoing_direction,
                              const Vector3F& incident_direction) const = 0;

    virtual Spectrum sample(const Vector3F& outgoing_direction,
                            Vector3F* incident_direction,
                            const Point2F& sample_values,
                            imp_float* pdf_value,
                            BXDFType* sampled_type = nullptr) const;

    virtual Spectrum reduced(const Vector3F& outgoing_direction,
                             unsigned int n_samples,
                             const Point2F* samples) const;

    virtual Spectrum reduced(unsigned int n_samples,
                             const Point2F* samples_1,
                             const Point2F* samples_2) const;

    virtual imp_float pdf(const Vector3F& outgoing_direction,
                          const Vector3F& incident_direction) const;
};

// ScaledBXDF declarations

class ScaledBXDF : public BXDF {

private:

    BXDF* bxdf; // The underlying BXDF
    Spectrum scale; // Scaling factor to apply to the BXDF properties

public:

    ScaledBXDF(BXDF* bxdf, const Spectrum& scale);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& sample_values,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    Spectrum reduced(const Vector3F& outgoing_direction,
                     unsigned int n_samples,
                     const Point2F* samples) const;

    Spectrum reduced(unsigned int n_samples,
                     const Point2F* samples_1,
                     const Point2F* samples_2) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

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

// SpecularBTDF declarations

class SpecularBTDF : public BXDF {

private:

    const TransmissionSpectrum transmittance; // Fraction of incident light that is transmitted (disregarding Fresnel effects)
    const imp_float refractive_index_outside; // Index of refraction of the medium on the outside of the object
    const imp_float refractive_index_inside; // Index of refraction of the medium on the inside of the object
    const DielectricReflector dielectric_reflector; // Dielectric Fresnel reflector for the surface
    const TransportMode transport_mode;

public:

    SpecularBTDF(const TransmissionSpectrum& transmittance,
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

// LambertianBRDF declarations

class LambertianBRDF : public BXDF {

private:
    const ReflectionSpectrum reflectance; // Fraction of incident light that is reflected

public:

    LambertianBRDF(const ReflectionSpectrum& reflectance);

    Spectrum evaluate(const Vector3F& outgoing_direction,
                      const Vector3F& incident_direction) const;

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& sample_values,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    Spectrum reduced(const Vector3F& outgoing_direction,
                     unsigned int n_samples,
                     const Point2F* samples) const;

    Spectrum reduced(unsigned int n_samples,
                     const Point2F* samples_1,
                     const Point2F* samples_2) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

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
                    const Point2F& sample_values,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction) const;
};

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

// BSDF declarations

class BSDF {

private:

    friend class MixedMaterial;
    
    const Normal3F geometric_normal; // Geometric surface normal

    const Normal3F shading_normal; // Shading surface normal
    const Vector3F shading_tangent; // Shading surface tangent in the primary direction
    const Vector3F shading_bitangent; // Shading surface tangent in the secondary direction

    static constexpr unsigned int max_bxdfs = 8; // Maxiumum number of BXDF components that a BSDF can consist of
    unsigned int n_bxdfs = 0; // Current number of BXDF components representing the BSDF
    BXDF* bxdfs[max_bxdfs]; // BXDF components representing the BSDF

    // The destructor is private because BSDFs should be (de)allocated through a RegionAllocator, not "new" and "delete"
    ~BSDF() {}

public:

    const imp_float refractive_index_outside; // Index of refraction of the medium on the outside of the object

    BSDF(const SurfaceScatteringEvent& scattering_event,
         imp_float refractive_index_outside = 1.0f);

    void addComponent(BXDF* bxdf);

    unsigned int numberOfComponents(BXDFType type = BSDF_ALL) const;

    Vector3F worldToLocal(const Vector3F& vector) const;
    Vector3F localToWorld(const Vector3F& vector) const;

    Spectrum evaluate(const Vector3F& world_outgoing_direction,
                      const Vector3F& world_incident_direction,
                      BXDFType type) const;

    Spectrum reduced(const Vector3F& outgoing_direction,
                     unsigned int n_samples,
                     const Point2F* samples,
                     BXDFType type = BSDF_ALL) const;

    Spectrum reduced(unsigned int n_samples,
                     const Point2F* samples_1,
                     const Point2F* samples_2,
                     BXDFType type = BSDF_ALL) const;

    Spectrum sample(const Vector3F& outgoing_direction,
                    Vector3F* incident_direction,
                    const Point2F& sample_values,
                    imp_float* pdf_value,
                    BXDFType type = BSDF_ALL) const;
};

// BSDF coordinate system utility functions

/*
Theta is the angle between the surface normal (z-axis) and the direction vector.
Phi is the counter-clockwise angle between the first surface tangent (x-axis) and
the projection of the direction vector in the tangent plane (xy-plane).
*/

inline imp_float cosTheta(const Vector3F& direction)
{
    return direction.z;
}

inline imp_float absCosTheta(const Vector3F& direction)
{
    return std::abs(direction.z);
}

inline imp_float cosSquaredTheta(const Vector3F& direction)
{
    return direction.z*direction.z;
}

inline imp_float sinSquaredTheta(const Vector3F& direction)
{
    return std::max<imp_float>(0, 1 - cosSquaredTheta(direction));
}

inline imp_float sinTheta(const Vector3F& direction)
{
    return std::sqrt(sinSquaredTheta(direction));
}

inline imp_float tanTheta(const Vector3F& direction)
{
    return sinTheta(direction)/cosTheta(direction);
}

inline imp_float tanSquaredTheta(const Vector3F& direction)
{
    return sinSquaredTheta(direction)/cosSquaredTheta(direction);
}

inline imp_float cosPhi(const Vector3F& direction)
{
    imp_float sin_theta = sinTheta(direction);
    return (sin_theta == 0)? 1 : clamp(direction.x/sin_theta, -1.0f, 1.0f);
}

inline imp_float cosSquaredPhi(const Vector3F& direction)
{
    imp_float cos_phi = cosPhi(direction);
    return cos_phi*cos_phi;
}

inline imp_float sinPhi(const Vector3F& direction)
{
    imp_float sin_theta = sinTheta(direction);
    return (sin_theta == 0)? 1 : clamp(direction.y/sin_theta, -1.0f, 1.0f);
}

inline imp_float sinSquaredPhi(const Vector3F& direction)
{
    imp_float sin_phi = sinPhi(direction);
    return sin_phi*sin_phi;
}

inline imp_float cosDeltaPhi(const Vector3F& direction_1, const Vector3F& direction_2)
{
    return clamp((direction_1.x*direction_2.x + direction_1.y*direction_2.y)/
                 std::sqrt((direction_1.x*direction_1.x + direction_1.y*direction_1.y)*
                           (direction_2.x*direction_2.x + direction_2.y*direction_2.y)),
                 -1.0f, 1.0f);
}

inline bool sameHemisphere(const Vector3F& direction_1, const Vector3F& direction_2)
{
    return direction_1.z*direction_2.z > 0;
}

inline bool sameHemisphere(const Vector3F& direction, const Normal3F& normal)
{
    return direction.z*normal.z > 0;
}

} // RayImpact
} // Impact
