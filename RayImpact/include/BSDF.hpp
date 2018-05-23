#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include "math.hpp"
#include "error.hpp"
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
                            const Point2F& uniform_sample,
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
                    const Point2F& uniform_sample,
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
                      BXDFType type = BSDF_ALL) const;

    Spectrum reduced(const Vector3F& outgoing_direction,
                     unsigned int n_samples,
                     const Point2F* samples,
                     BXDFType type = BSDF_ALL) const;

    Spectrum reduced(unsigned int n_samples,
                     const Point2F* samples_1,
                     const Point2F* samples_2,
                     BXDFType type = BSDF_ALL) const;

    Spectrum sample(const Vector3F& world_outgoing_direction,
                    Vector3F* world_incident_direction,
                    const Point2F& uniform_sample,
                    imp_float* pdf_value,
                    BXDFType type = BSDF_ALL,
					BXDFType* sampled_type = nullptr) const;

    imp_float pdf(const Vector3F& outgoing_direction,
                  const Vector3F& incident_direction,
                  BXDFType type = BSDF_ALL) const;
};

// BSDF inline function definitions

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

// BXDF inline method definitions

inline BXDF::BXDF(BXDFType type)
    : type(type)
{}

inline bool BXDF::containedIn(BXDFType t) const
{
    return (type & t) == type;
}

inline bool BXDF::contains(BXDFType t) const
{
    return type & t;
}

inline imp_float BXDF::pdf(const Vector3F& outgoing_direction,
						   const Vector3F& incident_direction) const
{
    return sameHemisphere(outgoing_direction, incident_direction)? absCosTheta(incident_direction)*IMP_ONE_OVER_PI : 0;
}

// ScaledBXDF inline method definitions

inline ScaledBXDF::ScaledBXDF(BXDF* bxdf, const Spectrum& scale)
    : BXDF::BXDF(BXDFType(bxdf->type)),
      bxdf(bxdf),
      scale(scale)
{}

inline Spectrum ScaledBXDF::evaluate(const Vector3F& outgoing_direction,
									 const Vector3F& incident_direction) const
{
    return scale*bxdf->evaluate(outgoing_direction, incident_direction);
}

inline Spectrum ScaledBXDF::sample(const Vector3F& outgoing_direction,
								   Vector3F* incident_direction,
								   const Point2F& uniform_sample,
								   imp_float* pdf_value,
								   BXDFType* sampled_type /* = nullptr */) const
{
    return scale*bxdf->sample(outgoing_direction,
                              incident_direction,
                              uniform_sample,
                              pdf_value,
                              sampled_type);
}

inline Spectrum ScaledBXDF::reduced(const Vector3F& outgoing_direction,
								    unsigned int n_samples,
									const Point2F* samples) const
{
    return scale*bxdf->reduced(outgoing_direction, n_samples, samples);
}

inline Spectrum ScaledBXDF::reduced(unsigned int n_samples,
									const Point2F* samples_1,
									const Point2F* samples_2) const
{
    return scale*bxdf->reduced(n_samples, samples_1, samples_2);
}

inline imp_float ScaledBXDF::pdf(const Vector3F& outgoing_direction,
								 const Vector3F& incident_direction) const
{
    return bxdf->pdf(outgoing_direction, incident_direction);
}

// BSDF inline method definitions

inline BSDF::BSDF(const SurfaceScatteringEvent& scattering_event,
				  imp_float refractive_index_outside /* = 1.0f */)
    : refractive_index_outside(refractive_index_outside),
      geometric_normal(scattering_event.surface_normal),
      shading_normal(scattering_event.shading.surface_normal),
      shading_tangent(scattering_event.shading.dpdu.normalized()),
      shading_bitangent(Vector3F(shading_normal).cross(shading_tangent))
{}

inline void BSDF::addComponent(BXDF* bxdf)
{
    imp_assert(n_bxdfs < max_bxdfs);
    bxdfs[n_bxdfs++] = bxdf;
}

inline Vector3F BSDF::worldToLocal(const Vector3F& vector) const
{
    return Vector3F(shading_tangent.dot(vector), shading_bitangent.dot(vector), shading_normal.dot(vector));
}

inline Vector3F BSDF::localToWorld(const Vector3F& vector) const
{
    return Vector3F(shading_tangent.x*vector.x + shading_bitangent.x*vector.y + shading_normal.x*vector.z,
                    shading_tangent.y*vector.x + shading_bitangent.y*vector.y + shading_normal.y*vector.z,
                    shading_tangent.z*vector.x + shading_bitangent.z*vector.y + shading_normal.z*vector.z);
}

} // RayImpact
} // Impact
