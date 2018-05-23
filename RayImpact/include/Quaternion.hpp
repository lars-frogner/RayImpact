#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include <ostream>
#include <sstream>
#include <string>

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

	std::string toString() const;
};

// Quaternion function declarations

Quaternion slerp(const Quaternion& quat_1, const Quaternion& quat_2, imp_float weight);

// Quaternion inline method definitions

inline Quaternion::Quaternion()
    : imag(0, 0, 0), w(1)
{}

inline Quaternion::Quaternion(const Vector3F& imag, imp_float w)
    : imag(imag), w(w)
{}

inline Quaternion::Quaternion(imp_float x, imp_float y, imp_float z, imp_float w)
    : imag(x, y, z), w(w)
{}

inline Quaternion Quaternion::operator+(const Quaternion& other) const
{
    return Quaternion(imag + other.imag, w + other.w);
}

inline Quaternion Quaternion::operator-(const Quaternion& other) const
{
    return Quaternion(imag - other.imag, w - other.w);
}

inline Quaternion Quaternion::operator*(const Quaternion& other) const
{
    return Quaternion(imag.cross(other.imag) + other.imag*w + imag*other.w,
                      w*other.w - imag.dot(other.imag));
}

inline Quaternion Quaternion::operator-() const
{
    return Quaternion(-imag, -w);
}

inline Quaternion Quaternion::operator*(imp_float factor) const
{
    return Quaternion(imag*factor, w*factor);
}

inline Quaternion Quaternion::operator/(imp_float divisor) const
{
    imp_float factor = 1.0f/divisor;
    return Quaternion(imag*factor, w*factor);
}

inline Quaternion& Quaternion::operator+=(const Quaternion& other)
{
    imag += other.imag;
    w += other.w;
    return *this;
}

inline Quaternion& Quaternion::operator-=(const Quaternion& other)
{
    imag -= other.imag;
    w -= other.w;
    return *this;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& other)
{
    imp_float imag_dot_prod = imag.dot(other.imag);
    imag = imag.cross(other.imag) + other.imag*w + imag*other.w;
    w = w*other.w - imag_dot_prod;
    return *this;
}

inline Quaternion& Quaternion::operator*=(imp_float factor)
{
    imag *= factor;
    w *= factor;
    return *this;
}

inline Quaternion& Quaternion::operator/=(imp_float divisor)
{
    imp_float factor = 1.0f/divisor;
    imag *= factor;
    w *= factor;
    return *this;
}

inline imp_float Quaternion::dot(const Quaternion& other) const
{
    return imag.dot(other.imag) + w*other.w;
}

inline Quaternion Quaternion::normalized() const
{
    return *this/dot(*this);
}

inline std::string Quaternion::toString() const
{
    std::ostringstream stream;
	stream << "{imag: " << imag << ", w: " << w << "}";
	return stream.str();
}

// Quaternion inline function definitions

inline std::ostream& operator<<(std::ostream& stream, const Quaternion& quaternion)
{
    stream << quaternion.toString();
    return stream;
}

} // RayImpact
} // Impact
