#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// Theta is the angle with respect to the positive z-axis,
// and phi is the angle in the xy-plane with respect to the
// positive x-axis. The (x, y, z) systemn is right-handed.

// Utility functions for spherical coordinates

// Returns the direction vector correponding to the given spherical coordinates
inline Vector3F sphericalToDirection(imp_float cos_theta,
                                     imp_float sin_theta,
                                     imp_float phi)
{
    return Vector3F(sin_theta*std::cos(phi), sin_theta*std::sin(phi), cos_theta);
}

// Returns the direction vector correponding to the given spherical coordinates in the given basis
inline Vector3F sphericalToDirection(imp_float cos_theta,
                                     imp_float sin_theta,
                                     imp_float phi,
                                     const Vector3F& x_axis,
                                     const Vector3F& y_axis,
                                     const Vector3F& z_axis)
{
    return x_axis*(sin_theta*std::cos(phi)) + y_axis*(sin_theta*std::sin(phi)) + z_axis*cos_theta;
}

// Returns the spherical theta-coordinate of the given direction vector
inline imp_float sphericalTheta(const Vector3F& direction)
{
    return std::acos(clamp(direction.z, -1.0f, 1.0f));
}

// Returns the spherical phi-coordinate of the given direction vector
inline imp_float sphericalPhi(const Vector3F& direction)
{
    if (direction.x == 0 && direction.y == 0)
        return 0.0f;

    imp_float phi = std::atan2(direction.y, direction.x);

    return (phi < 0)? (phi + IMP_TWO_PI) : phi;
}

} // RayImpact
} // Impact
