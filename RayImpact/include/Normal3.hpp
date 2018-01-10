#pragma once
#include "precision.hpp"
#include "error.hpp"
#include "Vector3.hpp"
#include <cmath>
#include <algorithm>
#include <ostream>

namespace Impact {
namespace RayImpact {

// Normal3 declarations

template <typename T>
class Normal3 {

private:
	
	bool hasNaNs() const;

public:

	T x, y, z; // Component values

	Normal3();
	Normal3(T x, T y, T z);

	explicit Normal3(const Vector3<T>& vector);

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);

	Normal3<T> operator+(const Normal3<T>& other) const;
	Normal3<T> operator-(const Normal3<T>& other) const;
	Normal3<T> operator*(T factor) const;
	Normal3<T> operator/(T divisor) const;
	
	Normal3<T> operator-() const;

	Normal3<T>& operator+=(const Normal3<T>& other);
	Normal3<T>& operator-=(const Normal3<T>& other);
	Normal3<T>& operator*=(T factor);
	Normal3<T>& operator/=(T divisor);

	T dot(const Normal3<T>& other) const;
	T dot(const Vector3<T>& vector) const;

	T absDot(const Normal3<T>& other) const;
	T absDot(const Vector3<T>& vector) const;

	T squaredLength() const;
	T length() const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;

	bool nonZero() const;

	Normal3<T> normalized() const;

	Normal3<T> permuted(unsigned int i,
						unsigned int j,
						unsigned int k) const;

	void normalize();

	void reverse();

	void flipToSameHemisphereAs(const Normal3<T>& other);
	void flipToSameHemisphereAs(const Vector3<T>& vector);
};

// Normal3 typedefs

typedef Normal3<imp_float> Normal3F;

// Functions on Normal3 objects

template <typename T>
inline Normal3<T> operator*(T factor, const Normal3<T>& normal)
{
	return normal*factor;
}

template <typename T>
inline Normal3<T> abs(const Normal3<T>& normal)
{
	return Normal3<T>(std::abs(normal.x), std::abs(normal.y), std::abs(normal.z));
}

// Component-wise minimum
template <typename T>
inline Normal3<T> min(const Normal3<T>& normal_1, const Normal3<T>& normal_2)
{
	return Normal3<T>(std::min(normal_1.x, normal_2.x),
					  std::min(normal_1.y, normal_2.y),
					  std::min(normal_1.z, normal_2.z));
}

// Component-wise maximum
template <typename T>
inline Normal3<T> max(const Normal3<T>& normal_1, const Normal3<T>& normal_2)
{
	return Normal3<T>(std::max(normal_1.x, normal_2.x),
					  std::max(normal_1.y, normal_2.y),
					  std::max(normal_1.z, normal_2.z));
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Normal3<T>& normal)
{
	stream << "<" << normal.x << ", " << normal.y <<  ", " << normal.z << ">";
	return stream;
}

// Normal3 method implementations

template <typename T>
inline Normal3<T>::Normal3()
	: x(0), y(0), z(0)
{}

template <typename T>
inline Normal3<T>::Normal3(T x, T y, T z)
	: x(x), y(y), z(z)
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline Normal3<T>::Normal3(const Vector3<T>& vector)
	: x(vector.x), y(vector.y), z(vector.z)
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline bool Normal3<T>::hasNaNs() const
{
	return std::isnan(x) || std::isnan(y) || std::isnan(z);
}

template <typename T>
inline T Normal3<T>::operator[](unsigned int dimension) const
{
	imp_assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline T& Normal3<T>::operator[](unsigned int dimension)
{
	imp_assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator+(const Normal3<T>& other) const
{
	return Normal3<T>(x + other.x, y + other.y, z + other.z);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator-(const Normal3<T>& other) const
{
	return Normal3<T>(x - other.x, y - other.y, z - other.z);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator*(T factor) const
{
	return Normal3<T>(x*factor, y*factor, z*factor);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator/(T divisor) const
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	return Normal3<T>(x*factor, y*factor, z*factor);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator-() const
{
	return Normal3<T>(-x, -y, -z);
}

template <typename T>
inline Normal3<T>& Normal3<T>::operator+=(const Normal3<T>& other)
{
	x += other.x; y += other.y; z += other.z;
	return *this;
}

template <typename T>
inline Normal3<T>& Normal3<T>::operator-=(const Normal3<T>& other)
{
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}

template <typename T>
inline Normal3<T>& Normal3<T>::operator*=(T factor)
{
	x *= factor; y *= factor; z *= factor;
	return *this;
}

template <typename T>
inline Normal3<T>& Normal3<T>::operator/=(T divisor)
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	x *= factor; y *= factor; z *= factor;
	return *this;
}

template <typename T>
inline T Normal3<T>::dot(const Normal3<T>& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

template <typename T>
inline T Normal3<T>::dot(const Vector3<T>& vector) const
{
	return x*vector.x + y*vector.y + z*vector.z;
}

// Absoulte value of dot product
template <typename T>
inline T Normal3<T>::absDot(const Normal3<T>& other) const
{
	return std::abs(dot(other));
}

// Absoulte value of dot product
template <typename T>
inline T Normal3<T>::absDot(const Vector3<T>& vector) const
{
	return std::abs(dot(vector));
}

template <typename T>
inline T Normal3<T>::squaredLength() const
{
	return x*x + y*y + z*z;
}

template <typename T>
inline T Normal3<T>::length() const
{
	return std::sqrt(squaredLength());
}

template <typename T>
inline T Normal3<T>::minComponent() const
{
	return std::min(x, std::min(y, z));
}

template <typename T>
inline T Normal3<T>::maxComponent() const
{
	return std::max(x, std::max(y, z));
}

template <typename T>
inline unsigned int Normal3<T>::minDimension() const
{
	return (x <= y)? ((x <= z)? 0 : 2) : ((y <= z)? 1 : 2);
}

template <typename T>
inline unsigned int Normal3<T>::maxDimension() const
{
	return (x >= y)? ((x >= z)? 0 : 2) : ((y >= z)? 1 : 2);
}

template <typename T>
inline bool Normal3<T>::nonZero() const
{
	return x != 0 || y != 0 || z != 0;
}

template <typename T>
inline Normal3<T> Normal3<T>::normalized() const
{
	return (*this)/length();
}

template <typename T>
inline Normal3<T> Normal3<T>::permuted(unsigned int i,
									   unsigned int j,
									   unsigned int k) const
{
	return Normal3<T>(operator[](i), operator[](j), operator[](k));
}

template <typename T>
inline void Normal3<T>::normalize()
{
	*this /= length();
}

template <typename T>
inline void Normal3<T>::reverse()
{
	x = -x; y = -y; z = -z;
}

template <typename T>
inline void Normal3<T>::flipToSameHemisphereAs(const Normal3<T>& other)
{
	if (dot(other) < 0.0f)
		reverse();
}

template <typename T>
inline void Normal3<T>::flipToSameHemisphereAs(const Vector3<T>& vector)
{
	if (dot(vector) < 0.0f)
		reverse();
}

} // RayImpact
} // Impact
