#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include "math.hpp"
#include "Spectrum.hpp"
#include "ScatteringEvent.hpp"
#include <cmath>
#include <algorithm>

namespace Impact {
namespace RayImpact {

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
