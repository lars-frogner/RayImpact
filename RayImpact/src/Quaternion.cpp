#include "Quaternion.hpp"
#include "math.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// Quaternion function definitions

// Spherical linear interpolation between the two quaternions
Quaternion slerp(const Quaternion& quat_1, const Quaternion& quat_2, imp_float weight)
{
    imp_float cos_theta = quat_1.dot(quat_2);

    if (cos_theta > 0.9995f)
    {
        // Use linear interpolation if the quaternions are nearly parallel
        return (quat_1*(1 - weight) + quat_2*weight).normalized();
    }
    else
    {
        imp_float theta = std::acos(clamp(cos_theta, -1.0f, 1.0f));
        imp_float weighted_theta = weight*theta;

        const Quaternion& orthogonal_quat = (quat_2 - quat_1*cos_theta).normalized();

        return quat_1*std::cos(weighted_theta) + orthogonal_quat*std::sin(weighted_theta);
    }
}

} // RayImpact
} // Impact
