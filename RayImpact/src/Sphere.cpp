#include "Sphere.hpp"
#include "math.hpp"
#include "ErrorFloat.hpp"
#include "geometry.hpp"
#include "api.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// Sphere method definitions

BoundingBoxF Sphere::objectSpaceBoundingBox() const
{
    if (phi_max >= IMP_THREE_PI_OVER_TWO)
    {
        return BoundingBoxF(Point3F(-radius, y_min, -radius),
                            Point3F( radius, y_max,  radius));
    }
    else if (phi_max >= IMP_PI)
    {
        return BoundingBoxF(Point3F(std::sin(phi_max)*radius, y_min, -radius),
                            Point3F(                  radius, y_max,  radius));
    }
    else if (phi_max >= IMP_PI_OVER_TWO)
    {
        return BoundingBoxF(Point3F(                       0, y_min, std::cos(phi_max)*radius),
                            Point3F(std::sin(phi_max)*radius, y_max,                   radius));
    }
    else
    {
        return BoundingBoxF(Point3F(                       0, y_min,                        0),
                            Point3F(std::sin(phi_max)*radius, y_max, std::cos(phi_max)*radius));
    }
}

bool Sphere::intersect(const Ray& ray,
                       imp_float* intersection_distance,
                       SurfaceScatteringEvent* scattering_event,
                       bool test_alpha_texture /* = true */) const
{
    // Transform ray to object space

    Vector3F transformed_ray_origin_error;
    Vector3F transformed_ray_direction_error;

    const Ray& transformed_ray = (*world_to_object)(ray, &transformed_ray_origin_error, &transformed_ray_direction_error);

    // Initialize copies of ray origin and direction with associated errors

    ErrorFloat origin_x(transformed_ray.origin.x, transformed_ray_origin_error.x);
    ErrorFloat origin_y(transformed_ray.origin.y, transformed_ray_origin_error.y);
    ErrorFloat origin_z(transformed_ray.origin.z, transformed_ray_origin_error.z);

    ErrorFloat direction_x(transformed_ray.direction.x, transformed_ray_direction_error.x);
    ErrorFloat direction_y(transformed_ray.direction.y, transformed_ray_direction_error.y);
    ErrorFloat direction_z(transformed_ray.direction.z, transformed_ray_direction_error.z);

    // Compute coefficients of the quadratic equation for the intersection distance
    const ErrorFloat& a = direction_x*direction_x + direction_y*direction_y + direction_z*direction_z;
    const ErrorFloat& b = 2.0f*(direction_x*origin_x + direction_y*origin_y + direction_z*origin_z);
    const ErrorFloat& c = origin_x*origin_x + origin_y*origin_y + origin_z*origin_z - ErrorFloat(radius)*ErrorFloat(radius);

    // Solve quadratic equation for intersection distance

    ErrorFloat shortest_intersect_dist; // Shortest tentative intersection distance
    ErrorFloat longest_intersect_dist; // Longest tentative intersection distance

    // The ray misses if there is no solution
    if (!solveQuadraticEquation(a, b, c, &shortest_intersect_dist, &longest_intersect_dist))
        return false;

    // Find relevant intersection distance

    // The ray misses if the intersection distances are outside the range (0, max_distance)
    if (shortest_intersect_dist.upperBound() > transformed_ray.max_distance ||
        longest_intersect_dist.lowerBound() < 0)
        return false;

    ErrorFloat relevant_intersect_dist = shortest_intersect_dist;

    if (relevant_intersect_dist.lowerBound() <= 0)
    {
        // Try the longest intersection distance if the ray origin is inside the sphere
        relevant_intersect_dist = longest_intersect_dist;

        if (relevant_intersect_dist.upperBound() > transformed_ray.max_distance)
        {
            return false;
        }
    }

    // Compute intersection point
    Point3F intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));

    // Project the intersection point onto the sphere (required in case the computed result has inaccuracies)
    intersection_point = intersection_point*(radius/distanceBetween(intersection_point, Point3F(0, 0, 0)));

    // Tweak z-coordinate if required to avoid error in atan2
    if (intersection_point.x == 0 && intersection_point.z == 0)
        intersection_point.z = 1e-5f*radius;

    // Compute azimuthal angle of intersection point
    imp_float intersection_phi = std::atan2(intersection_point.x, intersection_point.z);

    // Remap from [-pi, pi] to [0, 2*pi]
    if (intersection_phi < 0)
        intersection_phi += IMP_TWO_PI;

    // Check whether the intersection point is outside the parameter bounds
    if ((y_min > -radius && intersection_point.y < y_min) ||
        (y_max < radius && intersection_point.y > y_max) ||
        intersection_phi > phi_max)
    {
        // If the intersection point was the furthest one, the ray misses the partial sphere
        if (relevant_intersect_dist == longest_intersect_dist)
            return false;

        // At this point we know that the closest intersection point does not lie
        // on the partial sphere, but the furthest intersection point still might

        // If the longest intersection distance is too far away, the ray misses
        if (longest_intersect_dist.upperBound() > transformed_ray.max_distance)
            return false;

        // We now have to try again with the furthest intersection point

        relevant_intersect_dist = longest_intersect_dist;

        intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));

        intersection_point = intersection_point*(radius/distanceBetween(intersection_point, Point3F(0, 0, 0)));

        if (intersection_point.x == 0 && intersection_point.z == 0)
            intersection_point.z = 1e-5f*radius;

        intersection_phi = std::atan2(intersection_point.x, intersection_point.z);

        if (intersection_phi < 0)
            intersection_phi += IMP_TWO_PI;

        if ((y_min > -radius && intersection_point.y < y_min) ||
            (y_max < radius && intersection_point.y > y_max) ||
            intersection_phi > phi_max)
        {
            return false;
        }
    }

    // Compute u and v coordinates of the intersection point

    imp_float theta_range = theta_max - theta_min;
    imp_float intersection_theta = std::acos(clamp(intersection_point.y/radius, -1.0f, 1.0f));

    imp_float u = intersection_phi/phi_max;
    imp_float v = (intersection_theta - theta_min)/theta_range;

    // Compute u and v derivatives of the intersection point

    imp_float inverse_zx_radius = 1.0f/std::sqrt(intersection_point.z*intersection_point.z + intersection_point.x*intersection_point.x);
    imp_float cos_phi = intersection_point.z*inverse_zx_radius;
    imp_float sin_phi = intersection_point.x*inverse_zx_radius;

    const Vector3F& dpdu = Vector3F(intersection_point.z*phi_max, 0, -intersection_point.x*phi_max);

    const Vector3F& dpdv = Vector3F(intersection_point.y*sin_phi, -radius*std::sin(intersection_theta), intersection_point.y*cos_phi)*theta_range;

    // Compute u and v derivatives of the surface normal

    const Vector3F& d2pdu2 = Vector3F(intersection_point.x, 0, intersection_point.z)*(-phi_max*phi_max);
    const Vector3F& d2pdudv = Vector3F(cos_phi, 0, -sin_phi)*(theta_range*phi_max*intersection_point.y);
    const Vector3F& d2pdv2 = Vector3F(intersection_point.x, intersection_point.y, intersection_point.z)*(-theta_range*theta_range);

    Normal3F dndu, dndv;

    computeNormalDerivatives(dpdu, dpdv,
                             d2pdu2, d2pdudv, d2pdv2,
                             &dndu, &dndv);

    // Compute error for intersection point
    const Vector3F& intersection_point_error = abs(static_cast<Vector3F>(intersection_point))*errorPowerBound(5);

    // Construct scattering event
    *scattering_event = (*object_to_world)(SurfaceScatteringEvent(intersection_point,
                                                                  intersection_point_error,
                                                                  Point2F(u, v),
                                                                  -transformed_ray.direction,
                                                                  dpdu, dpdv,
                                                                  dndu, dndv,
                                                                  transformed_ray.time,
                                                                  this));

    // Set intersection distance (is correct for world space even though it was computed for object space)
    *intersection_distance = static_cast<imp_float>(relevant_intersect_dist);

    return true;
}

bool Sphere::hasIntersection(const Ray& ray,
                             bool test_alpha_texture /* = true */) const
{
    // Transform ray to object space

    Vector3F transformed_ray_origin_error;
    Vector3F transformed_ray_direction_error;

    const Ray& transformed_ray = (*world_to_object)(ray, &transformed_ray_origin_error, &transformed_ray_direction_error);

    // Initialize copies of ray origin and direction with associated errors

    ErrorFloat origin_x(transformed_ray.origin.x, transformed_ray_origin_error.x);
    ErrorFloat origin_y(transformed_ray.origin.y, transformed_ray_origin_error.y);
    ErrorFloat origin_z(transformed_ray.origin.z, transformed_ray_origin_error.z);

    ErrorFloat direction_x(transformed_ray.direction.x, transformed_ray_direction_error.x);
    ErrorFloat direction_y(transformed_ray.direction.y, transformed_ray_direction_error.y);
    ErrorFloat direction_z(transformed_ray.direction.z, transformed_ray_direction_error.z);

    // Compute coefficients of the quadratic equation for the intersection distance
    ErrorFloat a = direction_x*direction_x + direction_y*direction_y + direction_z*direction_z;
    ErrorFloat b = 2.0f*(direction_x*origin_x + direction_y*origin_y + direction_z*origin_z);
    ErrorFloat c = origin_x*origin_x + origin_y*origin_y + origin_z*origin_z - ErrorFloat(radius)*ErrorFloat(radius);

    // Solve quadratic equation for intersection distance

    ErrorFloat shortest_intersect_dist; // Shortest tentative intersection distance
    ErrorFloat longest_intersect_dist; // Longest tentative intersection distance

    // The ray misses if there is no solution
    if (!solveQuadraticEquation(a, b, c, &shortest_intersect_dist, &longest_intersect_dist))
        return false;

    // Find relevant intersection distance

    // The ray misses if the intersection distances are outside the range (0, max_distance)
    if (shortest_intersect_dist.upperBound() > transformed_ray.max_distance ||
        longest_intersect_dist.lowerBound() < 0)
        return false;

    ErrorFloat relevant_intersect_dist = shortest_intersect_dist;

    if (relevant_intersect_dist.lowerBound() <= 0)
    {
        // Try the longest intersection distance if the ray origin is inside the sphere
        relevant_intersect_dist = longest_intersect_dist;

        if (relevant_intersect_dist.upperBound() > transformed_ray.max_distance)
        {
            return false;
        }
    }

    // Compute intersection point
    Point3F intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));

    // Project the intersection point onto the sphere (required in case the computed result has inaccuracies)
    intersection_point = intersection_point*(radius/distanceBetween(intersection_point, Point3F(0, 0, 0)));

    imp_float intersection_phi;

    if (phi_max < IMP_TWO_PI)
    {
        // Tweak z-coordinate if required to avoid error in atan2
        if (intersection_point.x == 0 && intersection_point.z == 0)
            intersection_point.z = 1e-5f*radius;

        // Compute azimuthal angle of intersection point
        intersection_phi = std::atan2(intersection_point.x, intersection_point.z);

        // Remap from [-pi, pi] to [0, 2*pi]
        if (intersection_phi < 0)
            intersection_phi += IMP_TWO_PI;
    }
    else
    {
        intersection_phi = IMP_TWO_PI;
    }

    // Check whether the intersection point is outside the parameter bounds
    if ((y_min > -radius && intersection_point.y < y_min) ||
        (y_max < radius && intersection_point.y > y_max) ||
        intersection_phi > phi_max)
    {
        // If the intersection point was the furthest one, the ray misses the partial sphere
        if (relevant_intersect_dist == longest_intersect_dist)
            return false;

        // At this point we know that the closest intersection point does not lie
        // on the partial sphere, but the furthest intersection point still might

        // If the longest intersection distance is too far away, the ray misses
        if (longest_intersect_dist.upperBound() > transformed_ray.max_distance)
            return false;

        // We now have to try again with the furthest intersection point

        relevant_intersect_dist = longest_intersect_dist;

        intersection_point = transformed_ray(static_cast<imp_float>(relevant_intersect_dist));

        intersection_point = intersection_point*(radius/distanceBetween(intersection_point, Point3F(0, 0, 0)));

        if (phi_max < IMP_TWO_PI)
        {
            // Tweak z-coordinate if required to avoid error in atan2
            if (intersection_point.x == 0 && intersection_point.z == 0)
                intersection_point.z = 1e-5f*radius;

            // Compute azimuthal angle of intersection point
            intersection_phi = std::atan2(intersection_point.x, intersection_point.z);

            // Remap from [-pi, pi] to [0, 2*pi]
            if (intersection_phi < 0)
                intersection_phi += IMP_TWO_PI;
        }
        else
        {
            return false;
        }

        if ((y_min > -radius && intersection_point.y < y_min) ||
            (y_max < radius && intersection_point.y > y_max) ||
            intersection_phi > phi_max)
        {
            return false;
        }
    }

    return true;
}

// Sphere function definitions

std::shared_ptr<Shape> createSphere(const Transformation* object_to_world,
                                    const Transformation* world_to_object,
                                    bool has_reverse_orientation,
                                    const ParameterSet& parameters)
{
    imp_float radius = parameters.getSingleFloatValue("radius", 1.0f);
    imp_float bottom = parameters.getSingleFloatValue("bottom", -1.0f);
    imp_float top = parameters.getSingleFloatValue("top", 1.0f);
    imp_float sweep_angle = parameters.getSingleFloatValue("sweep_angle", 360.0f);
	
	if (RIMP_OPTIONS.verbosity >= IMP_SHAPES_VERBOSITY)
	{
		printInfoMessage("Shape:"
						 "\n    %-20s%s"
						 "\n    %-20s%g m"
						 "\n    %-20s%g m"
						 "\n    %-20s%g m"
						 "\n    %-20s%g degrees"
						 "\n    %-20s%s m"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Type:", "Sphere",
						 "Radius:", radius,
						 "Bottom:", bottom,
						 "Top:", top,
						 "Sweep angle:", sweep_angle,
						 "Center:", (*object_to_world)(Point3F(0, 0, 0)).toString().c_str(),
						 "Up direction:", (*object_to_world)(Vector3F(0, 1, 0)).toString().c_str(),
						 "Forward direction:", (*object_to_world)(Vector3F(0, 0, 1)).toString().c_str());
	}

    return std::make_shared<Sphere>(object_to_world,
                                    world_to_object,
                                    has_reverse_orientation,
                                    radius,
                                    bottom*radius, top*radius,
                                    sweep_angle);
}

} // RayImpact
} // Impact
