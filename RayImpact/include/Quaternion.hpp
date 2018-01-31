#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include <ostream>

namespace Impact {
namespace RayImpact {

// Quaternion declarations

class Quaternion {

public:
    Vector3F imag; // Imaginary components of the quaternion
    imp_float w; // Real component of the quaternion

    Quaternion();
    
    Quaternion(const Vector3F& imag, imp_float w);

    Quaternion(imp_float x, imp_float y, imp_float z, imp_float w);
    
    Quaternion operator+(const Quaternion& other) const;
    Quaternion operator-(const Quaternion& other) const;
    Quaternion operator*(const Quaternion& other) const;
    
    Quaternion operator-() const;

    Quaternion operator*(imp_float factor) const;
    Quaternion operator/(imp_float divisor) const;
    
    Quaternion& operator+=(const Quaternion& other);
    Quaternion& operator-=(const Quaternion& other);
    Quaternion& operator*=(const Quaternion& other);

    Quaternion& operator*=(imp_float factor);
    Quaternion& operator/=(imp_float divisor);

    imp_float dot(const Quaternion& other) const;

    Quaternion normalized() const;
};

// Functions on Quaternion objects

Quaternion slerp(const Quaternion& quat_1, const Quaternion& quat_2, imp_float weight);

std::ostream& operator<<(std::ostream& stream, const Quaternion& ray);

} // RayImpact
} // Impact
