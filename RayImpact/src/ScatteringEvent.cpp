#include "ScatteringEvent.hpp"
#include "Shape.hpp"
#include "Model.hpp"
#include "BSDF.hpp"
#include "math.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// ScatteringEvent method implementations

ScatteringEvent::ScatteringEvent()
    : position(),
      position_error(),
      outgoing_direction(),
      surface_normal(),
      medium_interface(nullptr),
      time(0)
{}

ScatteringEvent::ScatteringEvent(const Point3F& position,
                                 const Vector3F& position_error,
                                  const Vector3F& outgoing_direction,
                                 const Normal3F& surface_normal,
                                 const MediumInterface* medium_interface,
                                 imp_float time)
    : position(position),
      position_error(position_error),
      outgoing_direction(outgoing_direction),
      surface_normal(surface_normal),
      medium_interface(medium_interface),
      time(time)
{}

Ray ScatteringEvent::spawnRay(const Vector3F& direction) const
{
    const Point3F& origin = offsetRayOrigin(position, position_error, surface_normal, direction);

    return Ray(origin, direction, IMP_INFINITY, time, getMediumInDirection(direction));
}

Ray ScatteringEvent::spawnRayTo(const Point3F& end_point) const
{
    const Point3F& origin = offsetRayOrigin(position, position_error, surface_normal, end_point - position);

    const Vector3F& direction = end_point - origin;

    return Ray(origin, direction, 1.0f - IMP_SHADOW_EPS, time, getMediumInDirection(direction));
}

Ray ScatteringEvent::spawnRayTo(const ScatteringEvent& other) const
{
    return spawnRayTo(other.position);
}

bool ScatteringEvent::isOnSurface() const
{
    return surface_normal.nonZero();
}

// SurfaceScatteringEvent method implementations

SurfaceScatteringEvent::SurfaceScatteringEvent()
    : ScatteringEvent::ScatteringEvent(),
      position_uv(),
      dpdu(), dpdv(),
      dndu(), dndv(),
      dpdx(), dpdy(),
      dudx(0), dvdx(0), dudy(0), dvdy(0),
      shape(nullptr),
      model(nullptr),
      bsdf(nullptr),
      bssrdf(nullptr),
      shading()
{}

SurfaceScatteringEvent::SurfaceScatteringEvent(const Point3F& position,
                                               const Vector3F& position_error,
                                               const Point2F& position_uv,
                                               const Vector3F& outgoing_direction,
                                               const Vector3F& dpdu,
                                               const Vector3F& dpdv,
                                               const Normal3F& dndu,
                                               const Normal3F& dndv,
                                               imp_float time,
                                               const Shape* shape)
    : ScatteringEvent::ScatteringEvent(position,
                                       position_error,
                                       outgoing_direction,
                                       Normal3F(dpdu.cross(dpdv).normalized()),
                                       nullptr,
                                       time),
      position_uv(position_uv),
      dpdu(dpdu), dpdv(dpdv),
      dndu(dndu), dndv(dndv),
      dpdx(), dpdy(),
      dudx(0), dvdx(0), dudy(0), dvdy(0),
      shape(shape),
      model(nullptr),
      bsdf(nullptr),
      bssrdf(nullptr)
{
    shading.surface_normal = surface_normal;
    shading.dpdu = dpdu;
    shading.dpdv = dpdv;
    shading.dndu = dndu;
    shading.dndv = dndv;

    // Reverse surface normal if either the shape is specified to
    // have reverse orientation or its transformation swaps handedness
    // (if both are true, they cancel and the normal remains unchanged)
    if (shape && (shape->has_reverse_orientation ^ shape->transformation_swaps_handedness))
    {
        surface_normal.reverse();
        shading.surface_normal.reverse();
    }
}

void SurfaceScatteringEvent::setShadingGeometry(const Vector3F& dpdu,
                                                const Vector3F& dpdv,
                                                const Normal3F& dndu,
                                                const Normal3F& dndv,
                                                bool shading_normal_determines_orientation)
{
    shading.dpdu = dpdu;
    shading.dpdv = dpdv;
    shading.dndu = dndu;
    shading.dndv = dndv;

    shading.surface_normal = Normal3F(dpdu.cross(dpdv).normalized());

    if (shape && (shape->has_reverse_orientation ^ shape->transformation_swaps_handedness))
    {
        shading.surface_normal.reverse();
    }

    if (shading_normal_determines_orientation)
    {
        surface_normal.flipToSameHemisphereAs(shading.surface_normal);
    }
    else
    {
        shading.surface_normal.flipToSameHemisphereAs(surface_normal);
    }
}

void SurfaceScatteringEvent::computeScreenSpaceDerivatives(const RayWithOffsets& ray) const
{
    if (ray.has_offsets)
    {
        imp_float normal_distance = surface_normal.dot(static_cast<Vector3F>(position));

        // Estimate the point where the x-offset ray intersects the surface

        imp_float x_offset_intersect_dist = (normal_distance - surface_normal.dot(static_cast<Vector3F>(ray.x_offset_ray_origin)))
                                             /(surface_normal.dot(ray.x_offset_ray_direction));

        const Point3F& x_offset_intersect_pos = ray.x_offset_ray_origin + ray.x_offset_ray_direction*x_offset_intersect_dist;

        // Estimate the point where the y-offset ray intersects the surface
        
        imp_float y_offset_intersect_dist = (normal_distance - surface_normal.dot(static_cast<Vector3F>(ray.y_offset_ray_origin)))
                                             /(surface_normal.dot(ray.y_offset_ray_direction));

        const Point3F& y_offset_intersect_pos = ray.y_offset_ray_origin + ray.y_offset_ray_direction*y_offset_intersect_dist;

        // Compute derivatives of the scattering event position with respect to screen space x and y
        dpdx = x_offset_intersect_pos - position;
        dpdy = y_offset_intersect_pos - position;

        // We have dpdx = dpdu*dudx + dpdv*dvdx, so dudx and dvdx can be found by solving
        // this system of three equations. The system is overconstrained, so we ignore the
        // equation for the dimension were the component of dpdu x dpdv = n is largest.
        // The corresponding system with dpdy gives dudy and dvdy.

        // Determine which two dimensions to use when solving for the parametric derivatives

        unsigned int dimensions[2];

        if (std::abs(surface_normal.x) > std::abs(surface_normal.y) && std::abs(surface_normal.x) > std::abs(surface_normal.z))
        {
            // x-component is largest, so use y- and z-dimension
            dimensions[0] = 1;
            dimensions[1] = 2;
        }
        else if (std::abs(surface_normal.y) > std::abs(surface_normal.z))
        {
            // y-component is largest, so use x- and z-dimension
            dimensions[0] = 0;
            dimensions[1] = 2;
        }
        else
        {
            // z-component is largest, so use x- and y-dimension
            dimensions[0] = 0;
            dimensions[1] = 1;
        }

        // Set coeffiecients for the two systems of equations 
        imp_float coeffs[2][2] = {{dpdu[dimensions[0]], dpdv[dimensions[0]]},
                                  {dpdu[dimensions[1]], dpdv[dimensions[1]]}};

        // Set right-hand sides for the system of equations for dudx and dvdx
        imp_float rhs_x[2] = {dpdx[dimensions[0]], dpdx[dimensions[1]]};
        
        // Set right-hand sides for the system of equations for dudy and dvdy
        imp_float rhs_y[2] = {dpdy[dimensions[0]], dpdy[dimensions[1]]};

        // Solve for dudx and dvdx
        if (!solve2x2LinearSystem(coeffs, rhs_x, &dudx, &dvdx))
        {
            dudx = 0;
            dvdx = 0;
        }
        
        // Solve for dudy and dvdy
        if (!solve2x2LinearSystem(coeffs, rhs_y, &dudy, &dvdy))
        {
            dudy = 0;
            dvdy = 0;
        }
    }
    else
    {
        dpdx = Vector3F(0, 0, 0);
        dpdy = Vector3F(0, 0, 0);
        dudx = 0;
        dvdx = 0;
        dudy = 0;
        dvdy = 0;
    }
}

void SurfaceScatteringEvent::generateBSDF(const RayWithOffsets& ray,
                                          RegionAllocator& allocator,
                                          TransportMode transport_mode,
                                          bool allow_multiple_scattering_types)
{
    computeScreenSpaceDerivatives(ray);

    model->generateBSDF(this, allocator, transport_mode, allow_multiple_scattering_types);
}

// Utility functions

// Returns the position of a new ray origin that is guaranteed to not result in a false
// re-intersection of the surface while still being as close to the original position as possible
Point3F offsetRayOrigin(const Point3F& ray_origin,
                        const Vector3F& ray_origin_error,
                        const Normal3F& surface_normal,
                        const Vector3F& ray_direction)
{
    imp_float normal_offset_distance = ray_origin_error.dot(abs(surface_normal));
    Vector3F normal_offset = Vector3F(surface_normal)*normal_offset_distance;

    if (ray_direction.dot(surface_normal) < 0)
        normal_offset.reverse();

    Point3F offset_ray_origin = ray_origin + normal_offset;

    if (normal_offset.x > 0)
        offset_ray_origin.x = closestHigherFloat(offset_ray_origin.x);
    else if (normal_offset.x < 0)
        offset_ray_origin.x = closestLowerFloat(offset_ray_origin.x);

    if (normal_offset.y > 0)
        offset_ray_origin.y = closestHigherFloat(offset_ray_origin.y);
    else if (normal_offset.y < 0)
        offset_ray_origin.y = closestLowerFloat(offset_ray_origin.y);

    if (normal_offset.z > 0)
        offset_ray_origin.z = closestHigherFloat(offset_ray_origin.z);
    else if (normal_offset.z < 0)
        offset_ray_origin.z = closestLowerFloat(offset_ray_origin.z);

    return offset_ray_origin;
}

} // RayImpact
} // Impact
