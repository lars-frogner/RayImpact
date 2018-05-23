#pragma once
#include "precision.hpp"
#include "RegionAllocator.hpp"
#include "geometry.hpp"
#include "Ray.hpp"
#include "Spectrum.hpp"

namespace Impact {
namespace RayImpact {

// Forward declarations
class Shape;
class Model;
class BSDF;

// Offset from the end of a shadow ray to the area light the ray points towards
constexpr imp_float IMP_SHADOW_EPS = 0.0001f;

// ScatteringEvent declarations

class ScatteringEvent {

public:
    Point3F position; // Location of scattering event
    Vector3F position_error; // Error in scattering position
    Vector3F outgoing_direction; // Direction of the photon after scattering
    Normal3F surface_normal; // Normal vector for the surface (if present) at the scattering position
    MediumInterface medium_interface;
    imp_float time; // Point in time when the scattering event happens

    ScatteringEvent();

    ScatteringEvent(const Point3F& position,
                    const Vector3F& position_error,
                    const Vector3F& outgoing_direction,
                    const Normal3F& surface_normal,
                    const MediumInterface& medium_interface,
                    imp_float time);

    ScatteringEvent(const Point3F& position,
                    const MediumInterface& medium_interface,
                    imp_float time);

    Ray spawnRay(const Vector3F& direction) const;

    Ray spawnRayTo(const Point3F& end_point) const;

    Ray spawnRayTo(const ScatteringEvent& other) const;

    const Medium* getMediumInDirection(const Vector3F& direction) const {return nullptr;}

    bool isOnSurface() const;
};

// SurfaceScatteringEvent declarations

class SurfaceScatteringEvent : public ScatteringEvent {

public:

    Point2F position_uv; // Scattering position in terms of surface parameters u and v
    Vector3F dpdu; // Derivative of position with respect to u
    Vector3F dpdv; // Derivative of position with respect to v
    Normal3F dndu; // Derivative of surface normal with respect to u
    Normal3F dndv; // Derivative of surface normal with respect to v
    mutable Vector3F dpdx; // Derivative of position with respect to image plane coordinate x
    mutable Vector3F dpdy; // Derivative of position with respect to image plane coordinate y
    mutable imp_float dudx; // Derivative of u with respect to image plane coordinate x
    mutable imp_float dvdx; // Derivative of v with respect to image plane coordinate x
    mutable imp_float dudy; // Derivative of u with respect to image plane coordinate y
    mutable imp_float dvdy; // Derivative of v with respect to image plane coordinate y
    const Shape* shape; // Shape representing the surface
    const Model* model; // The model the surface belongs to
    BSDF* bsdf; // The BSDF associated with the surface
    BSSRDF* bssrdf; // The BSSRDF associated with the surface

    // Versions of surface normal and derivatives for lighting calculations
    struct
    {
        Normal3F surface_normal;
        Vector3F dpdu;
        Vector3F dpdv;
        Normal3F dndu;
        Normal3F dndv;

    } shading;

    SurfaceScatteringEvent();

    SurfaceScatteringEvent(const Point3F& position,
                           const Vector3F& position_error,
                           const Point2F& position_uv,
                           const Vector3F& outgoing_direction,
                           const Vector3F& dpdu,
                           const Vector3F& dpdv,
                           const Normal3F& dndu,
                           const Normal3F& dndv,
                           imp_float time,
                           const Shape* shape);

    void setShadingGeometry(const Vector3F& dpdu,
                            const Vector3F& dpdv,
                            const Normal3F& dndu,
                            const Normal3F& dndv,
                            bool shading_normal_determines_orientation);

    void computeScreenSpaceDerivatives(const RayWithOffsets& ray) const;

    void generateBSDF(const RayWithOffsets& ray,
                      RegionAllocator& allocator,
                      TransportMode transport_mode = TransportMode::Radiance,
                      bool allow_multiple_scattering_types = false);

    RadianceSpectrum emittedRadiance(const Vector3F& outgoing_direction) const;
};

// ScatteringEvent function declarations

Point3F offsetRayOrigin(const Point3F& ray_origin,
                        const Vector3F& ray_origin_error,
                        const Normal3F& surface_normal,
                        const Vector3F& ray_direction);

// ScatteringEvent inline method definitions

inline ScatteringEvent::ScatteringEvent()
    : position(),
      position_error(),
      outgoing_direction(),
      surface_normal(),
      medium_interface(),
      time(0)
{}

inline ScatteringEvent::ScatteringEvent(const Point3F& position,
										const Vector3F& position_error,
										const Vector3F& outgoing_direction,
										const Normal3F& surface_normal,
										const MediumInterface& medium_interface,
										imp_float time)
    : position(position),
      position_error(position_error),
      outgoing_direction(outgoing_direction),
      surface_normal(surface_normal),
      medium_interface(medium_interface),
      time(time)
{}

inline ScatteringEvent::ScatteringEvent(const Point3F& position,
										const MediumInterface& medium_interface,
										imp_float time)
    : position(position),
      position_error(),
      outgoing_direction(),
      surface_normal(),
      medium_interface(medium_interface),
      time(time)
{}

inline bool ScatteringEvent::isOnSurface() const
{
    return surface_normal.nonZero();
}

} // RayImpact
} // Impact
