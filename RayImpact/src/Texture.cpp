#include "Texture.hpp"
#include "math.hpp"
#include "spherical.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// ParametricMapper method implementations

ParametricMapper::ParametricMapper(imp_float s_scale, imp_float t_scale,
                                   imp_float s_offset, imp_float t_offset)
    : s_scale(s_scale), t_scale(t_scale),
      s_offset(s_offset), t_offset(t_offset)
{}

Point2F ParametricMapper::textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                                            Vector2F* dstdx, Vector2F* dstdy) const
{
    *dstdx = Vector2F(s_scale*scattering_event.dudx, t_scale*scattering_event.dvdx);
    *dstdx = Vector2F(s_scale*scattering_event.dudy, t_scale*scattering_event.dvdy);

    return Point2F(s_scale*scattering_event.position_uv.x + s_offset,
                   t_scale*scattering_event.position_uv.y + t_offset);
}

// SphericalMapper method implementations

SphericalMapper::SphericalMapper(const Transformation& world_to_sphere)
    : world_to_sphere(world_to_sphere)
{}

Point2F SphericalMapper::projectedToSphere(const Point3F& point) const
{
    const Vector3F& radius_vector = (world_to_sphere(point) - Point3F(0, 0, 0)).normalized();

    return Point2F(sphericalPhi(radius_vector)*IMP_ONE_OVER_TWO_PI,
                   sphericalTheta(radius_vector)*IMP_ONE_OVER_PI);
}

Point2F SphericalMapper::textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                                           Vector2F* dstdx, Vector2F* dstdy) const
{
    const Point2F& texture_coordinate = projectedToSphere(scattering_event.position);
    
    const imp_float delta = 0.1f;
    const imp_float inv_delta = 1.0f/delta;

    *dstdx = (projectedToSphere(scattering_event.position + delta*scattering_event.dpdx) - texture_coordinate)*inv_delta;
    *dstdy = (projectedToSphere(scattering_event.position + delta*scattering_event.dpdy) - texture_coordinate)*inv_delta;

    // Account for discontinuity in s due to the seam between phi = 2*pi and phi = 0

    if (dstdx->x > 0.5f)
        dstdx->x = 1 - dstdx->x;
    else if (dstdx->x < -0.5f)
        dstdx->x = -1 - dstdx->x;

    if (dstdy->x > 0.5f)
        dstdy->x = 1 - dstdy->x;
    else if (dstdy->x < -0.5f)
        dstdy->x = -1 - dstdy->x;

    return texture_coordinate;
}

// CylindricalMapper method implementations

CylindricalMapper::CylindricalMapper(const Transformation& world_to_cylinder)
    : world_to_cylinder(world_to_cylinder)
{}

Point2F CylindricalMapper::projectedToCylinder(const Point3F& point) const
{
    const Vector3F& radius_vector = (world_to_cylinder(point) - Point3F(0, 0, 0)).normalized();

    return Point2F((std::atan2(radius_vector.x, radius_vector.z) + IMP_PI)*IMP_ONE_OVER_TWO_PI,
                   radius_vector.y);
}

Point2F CylindricalMapper::textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                                             Vector2F* dstdx, Vector2F* dstdy) const
{
    const Point2F& texture_coordinate = projectedToCylinder(scattering_event.position);
    
    const imp_float delta = 0.1f;
    const imp_float inv_delta = 1.0f/delta;

    *dstdx = (projectedToCylinder(scattering_event.position + delta*scattering_event.dpdx) - texture_coordinate)*inv_delta;
    *dstdy = (projectedToCylinder(scattering_event.position + delta*scattering_event.dpdy) - texture_coordinate)*inv_delta;

    // Account for discontinuity in s due to the seam between phi = 2*pi and phi = 0

    if (dstdx->x > 0.5f)
        dstdx->x = 1 - dstdx->x;
    else if (dstdx->x < -0.5f)
        dstdx->x = -1 - dstdx->x;

    if (dstdy->x > 0.5f)
        dstdy->x = 1 - dstdy->x;
    else if (dstdy->x < -0.5f)
        dstdy->x = -1 - dstdy->x;

    return texture_coordinate;
}

// PlanarMapper method implementations

PlanarMapper::PlanarMapper(const Vector3F& s_tangent, const Vector3F& t_tangent,
                           imp_float s_offset /* = 0 */, imp_float t_offset /* = 0 */)
    : s_tangent(s_tangent), t_tangent(t_tangent),
      s_offset(s_offset), t_offset(t_offset)
{}

Point2F PlanarMapper::textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                                        Vector2F* dstdx, Vector2F* dstdy) const
{
    const Vector3F& position_vector = static_cast<Vector3F>(scattering_event.position);
    
    *dstdx = Vector2F(scattering_event.dpdx.dot(s_tangent), scattering_event.dpdx.dot(t_tangent));
    *dstdy = Vector2F(scattering_event.dpdy.dot(s_tangent), scattering_event.dpdy.dot(t_tangent));

    return Point2F(position_vector.dot(s_tangent) + s_offset,
                   position_vector.dot(t_tangent) + t_offset);
}

// TransformationMapper method implementations

TransformationMapper::TransformationMapper(const Transformation& mapping_transformation)
    : mapping_transformation(mapping_transformation)
{}

Point3F TransformationMapper::textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                                                Vector3F* dpdx, Vector3F* dpdy) const
{
    *dpdx = mapping_transformation(scattering_event.dpdx);
    *dpdy = mapping_transformation(scattering_event.dpdy);

    return mapping_transformation(scattering_event.position);
}

} // RayImpact
} // Impact
