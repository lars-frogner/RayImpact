#include "Quaternion.hpp"
#include "math.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// Quaternion method implementations

Quaternion::Quaternion()
    : imag(0, 0, 0), w(1)
{}

Quaternion::Quaternion(const Vector3F& imag, imp_float w)
    : imag(imag), w(w)
{}

Quaternion::Quaternion(imp_float x, imp_float y, imp_float z, imp_float w)
    : imag(x, y, z), w(w)
{}
    
Quaternion Quaternion::operator+(const Quaternion& other) const
{
    return Quaternion(imag + other.imag, w + other.w);
}

Quaternion Quaternion::operator-(const Quaternion& other) const
{
    return Quaternion(imag - other.imag, w - other.w);
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
    return Quaternion(imag.cross(other.imag) + other.imag*w + imag*other.w,
                      w*other.w - imag.dot(other.imag));
}
    
Quaternion Quaternion::operator-() const
{
    return Quaternion(-imag, -w);
}

Quaternion Quaternion::operator*(imp_float factor) const
{
    return Quaternion(imag*factor, w*factor);
}

Quaternion Quaternion::operator/(imp_float divisor) const
{
    imp_float factor = 1.0f/divisor;
    return Quaternion(imag*factor, w*factor);
}
    
Quaternion& Quaternion::operator+=(const Quaternion& other)
{
    imag += other.imag;
    w += other.w;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& other)
{
    imag -= other.imag;
    w -= other.w;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
    imp_float imag_dot_prod = imag.dot(other.imag);

    imag = imag.cross(other.imag) + other.imag*w + imag*other.w;
    w = w*other.w - imag_dot_prod;

    return *this;
}

Quaternion& Quaternion::operator*=(imp_float factor)
{
    imag *= factor;
    w *= factor;
    return *this;
}

Quaternion& Quaternion::operator/=(imp_float divisor)
{
    imp_float factor = 1.0f/divisor;
    imag *= factor;
    w *= factor;
    return *this;
}

imp_float Quaternion::dot(const Quaternion& other) const
{
    return imag.dot(other.imag) + w*other.w;
}

Quaternion Quaternion::normalized() const
{
    return *this/dot(*this);
}

// Functions on Quaternion objects

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

std::ostream& operator<<(std::ostream& stream, const Quaternion& quaternion)
{
    stream << "{imag = " << quaternion.imag << ", w = " << quaternion.w << "}";
    return stream;
}

} // RayImpact
} // Impact
