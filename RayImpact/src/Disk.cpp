#include "Disk.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include "api.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// Disk method definitions

BoundingBoxF Disk::objectSpaceBoundingBox() const
{
    if (phi_max >= IMP_THREE_PI_OVER_TWO)
    {
        return BoundingBoxF(Point3F(-radius, y, -radius),
                            Point3F( radius, y,  radius));
    }
    else if (phi_max >= IMP_PI)
    {
        return BoundingBoxF(Point3F(std::sin(phi_max)*radius, y, -radius),
                            Point3F(                  radius, y,  radius));
    }
    else if (phi_max >= IMP_PI_OVER_TWO)
    {
        return BoundingBoxF(Point3F(                       0, y, std::cos(phi_max)*radius),
                            Point3F(std::sin(phi_max)*radius, y,                   radius));
    }
    else
    {
        return BoundingBoxF(Point3F(                       0, y,                        0),
                            Point3F(std::sin(phi_max)*radius, y, std::cos(phi_max)*radius));
    }
}

bool Disk::intersect(const Ray& ray,
                     imp_float* intersection_distance,
                     SurfaceScatteringEvent* scattering_event,
                     bool test_alpha_texture /* = true */) const
{
    // Transform ray to object space

    Vector3F transformed_ray_origin_error;
    Vector3F transformed_ray_direction_error;

    const Ray& transformed_ray = (*world_to_object)(ray, &transformed_ray_origin_error, &transformed_ray_direction_error);

    // The ray misses if it points exactly along the disk plane
    if (transformed_ray.direction.y == 0)
        return false;

    // Compute distance to intersection with disk plane
    imp_float plane_intersection_distance = (y - transformed_ray.origin.y)/transformed_ray.direction.y;

    // The ray misses if the intersection distance is outside the range (0, max_distance)
    if (plane_intersection_distance > transformed_ray.max_distance ||
        plane_intersection_distance < 0)
        return false;

    // Compute intersection point
    Point3F intersection_point = transformed_ray(static_cast<imp_float>(plane_intersection_distance));

    // Compute squared distance from intersection to disk center in disk plane
    imp_float squared_intersection_radius = intersection_point.x*intersection_point.x + intersection_point.z*intersection_point.z;

    // The ray misses if the intersection is outside the valid range of radii
    if (squared_intersection_radius > radius*radius || squared_intersection_radius < inner_radius*inner_radius)
        return false;

    // Tweak z-coordinate if required to avoid error in atan2
    if (intersection_point.x == 0 && intersection_point.z == 0)
        intersection_point.z = 1e-5f*radius;

    // Compute azimuthal angle of intersection point
    imp_float intersection_phi = std::atan2(intersection_point.x, intersection_point.z);

    // Remap from [-pi, pi] to [0, 2*pi]
    if (intersection_phi < 0)
        intersection_phi += IMP_TWO_PI;

    if (intersection_phi > phi_max)
        return false;

    // Compute u and v coordinates of the intersection point

    imp_float intersection_radius = std::sqrt(squared_intersection_radius);
    imp_float radius_range = radius - inner_radius;

    imp_float u = intersection_phi/phi_max;
    imp_float v = 1 - (intersection_radius - inner_radius)/radius_range;

    // Compute u and v derivatives of the intersection point

    const Vector3F& dpdu = Vector3F(-intersection_point.z*phi_max, 0, intersection_point.x*phi_max);

    const Vector3F& dpdv = Vector3F(intersection_point.x, 0, intersection_point.z)*(-radius_range/intersection_radius);

    // dndu = (0, 0, 0)
    // dndv = (0, 0, 0)

    // Project the intersection point onto the disk (required in case the computed result has inaccuracies)
    intersection_point.y = y;

    // intersection_point_error = (0, 0, 0)

    // Construct scattering event
    *scattering_event = (*object_to_world)(SurfaceScatteringEvent(intersection_point,
                                                                  Vector3F(0, 0, 0),
                                                                  Point2F(u, v),
                                                                  -transformed_ray.direction,
                                                                  dpdu, dpdv,
                                                                  Normal3F(0, 0, 0), Normal3F(0, 0, 0),
                                                                  transformed_ray.time,
                                                                  this));

    // Set intersection distance (is correct for world space even though it was computed for object space)
    *intersection_distance = plane_intersection_distance;

    return true;
}

bool Disk::hasIntersection(const Ray& ray,
                           bool test_alpha_texture /* = true */) const
{
    // Transform ray to object space

    Vector3F transformed_ray_origin_error;
    Vector3F transformed_ray_direction_error;

    const Ray& transformed_ray = (*world_to_object)(ray, &transformed_ray_origin_error, &transformed_ray_direction_error);

    // The ray misses if it points exactly along the disk plane
    if (transformed_ray.direction.y == 0)
        return false;

    // Compute distance to intersection with disk plane
    imp_float plane_intersection_distance = (y - transformed_ray.origin.y)/transformed_ray.direction.y;

    // The ray misses if the intersection distance is outside the range (0, max_distance)
    if (plane_intersection_distance > transformed_ray.max_distance ||
        plane_intersection_distance < 0)
        return false;

    // Compute intersection point
    Point3F intersection_point = transformed_ray(static_cast<imp_float>(plane_intersection_distance));

    // Compute squared distance from intersection to disk center in disk plane
    imp_float squared_intersection_radius = intersection_point.x*intersection_point.x + intersection_point.z*intersection_point.z;

    // The ray misses if the intersection is outside the valid range of radii
    if (squared_intersection_radius > radius*radius || squared_intersection_radius < inner_radius*inner_radius)
        return false;

    if (phi_max < IMP_TWO_PI)
    {
        // Tweak z-coordinate if required to avoid error in atan2
        if (intersection_point.x == 0 && intersection_point.z == 0)
            intersection_point.z = 1e-5f*radius;

        // Compute azimuthal angle of intersection point
        imp_float intersection_phi = std::atan2(intersection_point.x, intersection_point.z);

        // Remap from [-pi, pi] to [0, 2*pi]
        if (intersection_phi < 0)
            intersection_phi += IMP_TWO_PI;

        if (intersection_phi > phi_max)
            return false;
    }

    return true;
}

// Disk function definitions

std::shared_ptr<Shape> createDisk(const Transformation* object_to_world,
                                  const Transformation* world_to_object,
                                  bool has_reverse_orientation,
                                  const ParameterSet& parameters)
{
    imp_float radius = parameters.getSingleFloatValue("radius", 1.0f);
    imp_float inner_radius = parameters.getSingleFloatValue("inner_radius", 0.0f);
    imp_float height = parameters.getSingleFloatValue("height", 0.0f);
    imp_float sweep_angle = parameters.getSingleFloatValue("sweep_angle", 360.0f);
	
	if (RIMP_OPTIONS.verbosity >= IMP_SHAPES_VERBOSITY)
	{
		printInfoMessage("Shape:"
						 "\n    %-20s%s"
						 "\n    %-20s%g m"
						 "\n    %-20s%g m"
						 "\n    %-20s%g degrees"
						 "\n    %-20s%s m"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Type:", "Disk",
						 "Radius:", radius,
						 "Inner radius:", inner_radius,
						 "Sweep angle:", sweep_angle,
						 "Center:", (*object_to_world)(Point3F(0, 0, 0)).toString().c_str(),
						 "Up direction:", (*object_to_world)(Vector3F(0, 1, 0)).toString().c_str(),
						 "Forward direction:", (*object_to_world)(Vector3F(0, 0, 1)).toString().c_str());
	}

    return std::make_shared<Disk>(object_to_world,
                                  world_to_object,
                                  has_reverse_orientation,
                                  radius, inner_radius,
                                  height,
                                  sweep_angle);
}

} // RayImpact
} // Impact
