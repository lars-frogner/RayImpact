#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include "math.hpp"
#include "Spectrum.hpp"
#include <cmath>
#include <algorithm>

namespace Impact {
namespace RayImpact {

// BSDF utility functions

imp_float fresnelReflectance(imp_float cos_incident_angle,
                             imp_float refractive_index_outside,
                             imp_float refractive_index_inside);

ReflectanceSpectrum fresnelReflectance(imp_float cos_incident_angle,
                                       const Spectrum& refractive_index_outside,
                                       const Spectrum& refractive_index_inside,
                                       const Spectrum& absorption_coefficient_inside);

bool refract(const Vector3F& incident_dir,
             const Normal3F& incident_surface_normal,
             imp_float refractive_index_incident_medium,
             imp_float refractive_index_transmitted_medium,
             Vector3F* transmitted_dir);

// FresnelReflector declarations

class FresnelReflector {

public:
    virtual ReflectanceSpectrum evaluate(imp_float cos_incident_angle) const = 0;
};

// PerfectReflector implementation

class PerfectReflector : public FresnelReflector {

public:

    ReflectanceSpectrum evaluate(imp_float cos_incident_angle) const
    {
        return ReflectanceSpectrum(1.0f);
    }
};

// DielectricReflector implementation

class DielectricReflector : public FresnelReflector {

private:
    imp_float refractive_index_outside;
    imp_float refractive_index_inside;

public:

    DielectricReflector(imp_float refractive_index_outside,
                        imp_float refractive_index_inside)
        : refractive_index_outside(refractive_index_outside),
          refractive_index_inside(refractive_index_inside)
    {}

    ReflectanceSpectrum evaluate(imp_float cos_incident_angle) const
    {
        return ReflectanceSpectrum(fresnelReflectance(cos_incident_angle,
                                                      refractive_index_outside,
                                                      refractive_index_inside));
    }
};

// ConductiveReflector implementation

class ConductiveReflector : public FresnelReflector {

private:
    Spectrum refractive_index_outside;
    Spectrum refractive_index_inside;
    Spectrum absorption_coefficient_inside;

public:

    ConductiveReflector(const Spectrum& refractive_index_outside,
                        const Spectrum& refractive_index_inside,
                        const Spectrum& absorption_coefficient_inside)
        : refractive_index_outside(refractive_index_outside),
          refractive_index_inside(refractive_index_inside),
          absorption_coefficient_inside(absorption_coefficient_inside)
    {}

    ReflectanceSpectrum operator()(imp_float cos_incident_angle) const
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

    bool hasType(BXDFType t) const;

    virtual Spectrum evaluate(const Vector3F& outgoing_dir,
                              const Vector3F& incident_dir) const = 0;

    virtual Spectrum sample(const Vector3F& outgoing_dir,
                            Vector3F* incident_dir,
                            const Point2F& sample_point,
                            imp_float* pdf_value,
                            BXDFType* sampled_type = nullptr) const;

    virtual Spectrum reduced(const Vector3F& outgoing_dir,
                             unsigned int n_samples,
                             const Point2F* samples) const;

    virtual Spectrum reduced(unsigned int n_samples,
                             const Point2F* samples_1,
                             const Point2F* samples_2) const;

    virtual imp_float pdf(const Vector3F& outgoing_dir,
                          const Vector3F& incident_dir) const;
};

// ScaledBXDF declarations

class ScaledBXDF : public BXDF {

private:

    BXDF* bxdf;
    Spectrum scale;

public:

    ScaledBXDF(BXDF* bxdf, const Spectrum& scale);

    Spectrum evaluate(const Vector3F& outgoing_dir,
                      const Vector3F& incident_dir) const;

    Spectrum sample(const Vector3F& outgoing_dir,
                    Vector3F* incident_dir,
                    const Point2F& sample_point,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    Spectrum reduced(const Vector3F& outgoing_dir,
                     unsigned int n_samples,
                     const Point2F* samples) const;

    Spectrum reduced(unsigned int n_samples,
                     const Point2F* samples_1,
                     const Point2F* samples_2) const;

    imp_float pdf(const Vector3F& outgoing_dir,
                  const Vector3F& incident_dir) const;
};

// SpecularBRDF declarations

class SpecularBRDF : public BXDF {

private:

    const ReflectanceSpectrum reflectance;
    const FresnelReflector* fresnel_reflector;

public:

    SpecularBRDF(const ReflectanceSpectrum& reflectance,
                 FresnelReflector* fresnel_reflector);

    Spectrum evaluate(const Vector3F& outgoing_dir,
                      const Vector3F& incident_dir) const;

    Spectrum sample(const Vector3F& outgoing_dir,
                    Vector3F* incident_dir,
                    const Point2F& sample_point,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;
};

// SpecularBTDF declarations

class SpecularBTDF : public BXDF {

private:

    const TransmittanceSpectrum transmittance;
    const imp_float refractive_index_outside;
    const imp_float refractive_index_inside;
    const DielectricReflector dielectric_reflector;
    const TransportMode transport_mode;

public:

    SpecularBTDF(const TransmittanceSpectrum& transmittance,
                 imp_float refractive_index_outside,
                 imp_float refractive_index_inside,
                 TransportMode transport_mode);

    Spectrum evaluate(const Vector3F& outgoing_dir,
                      const Vector3F& incident_dir) const;

    Spectrum sample(const Vector3F& outgoing_dir,
                    Vector3F* incident_dir,
                    const Point2F& sample_point,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;
};

// SpecularBSDF declarations

class SpecularBSDF : public BXDF {

private:
    
    const ReflectanceSpectrum reflectance;
    const TransmittanceSpectrum transmittance;
    const imp_float refractive_index_outside;
    const imp_float refractive_index_inside;
    const DielectricReflector dielectric_reflector;
    const TransportMode transport_mode;

public:

    SpecularBSDF(const ReflectanceSpectrum& reflectance,
                 const TransmittanceSpectrum& transmittance,
                 imp_float refractive_index_outside,
                 imp_float refractive_index_inside,
                 TransportMode transport_mode);

    Spectrum evaluate(const Vector3F& outgoing_dir,
                      const Vector3F& incident_dir) const;

    Spectrum sample(const Vector3F& outgoing_dir,
                    Vector3F* incident_dir,
                    const Point2F& sample_point,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;
};

// LambertianBRDF declarations

class LambertianBRDF : public BXDF {

private:
    const ReflectanceSpectrum reflectance;

public:

    LambertianBRDF(const ReflectanceSpectrum& reflectance);

    Spectrum evaluate(const Vector3F& outgoing_dir,
                      const Vector3F& incident_dir) const;

    Spectrum sample(const Vector3F& outgoing_dir,
                    Vector3F* incident_dir,
                    const Point2F& sample_point,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    Spectrum reduced(const Vector3F& outgoing_dir,
                     unsigned int n_samples,
                     const Point2F* samples) const;

    Spectrum reduced(unsigned int n_samples,
                     const Point2F* samples_1,
                     const Point2F* samples_2) const;

    imp_float pdf(const Vector3F& outgoing_dir,
                  const Vector3F& incident_dir) const;
};

// LambertianBTDF declarations

class LambertianBTDF : public BXDF {

private:
    const TransmittanceSpectrum transmittance;

public:

    LambertianBTDF(const TransmittanceSpectrum& transmittance);

    Spectrum evaluate(const Vector3F& outgoing_dir,
                      const Vector3F& incident_dir) const;

    Spectrum sample(const Vector3F& outgoing_dir,
                    Vector3F* incident_dir,
                    const Point2F& sample_point,
                    imp_float* pdf_value,
                    BXDFType* sampled_type = nullptr) const;

    imp_float pdf(const Vector3F& outgoing_dir,
                  const Vector3F& incident_dir) const;
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
