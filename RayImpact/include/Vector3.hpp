#pragma once
#include "precision.hpp"
#include "error.hpp"
#include <cmath>
#include <algorithm>
#include <ostream>

namespace Impact {
namespace RayImpact {

// Forward declarations

template <typename T>
class Normal3;

// Vector3 declarations

template <typename T>
class Vector3 {

private:
	
	bool hasNaNs() const;

public:

	T x, y, z; // Component values

	Vector3();
	Vector3(T x, T y, T z);

	explicit Vector3(const Normal3& normal);

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);

	Vector3 operator+(const Vector3& other) const;
	Vector3 operator-(const Vector3& other) const;
	Vector3 operator*(T factor) const;
	Vector3 operator/(T divisor) const;
	
	Vector3 operator-() const;

	Vector3& operator+=(const Vector3& other);
	Vector3& operator-=(const Vector3& other);
	Vector3& operator*=(T factor);
	Vector3& operator/=(T divisor);

	T dot(const Vector3& other) const;
	T dot(const Normal3& normal) const;

	T absDot(const Vector3& other) const;
	T absDot(const Normal3& normal) const;
	
	Vector3 cross(const Vector3& other) const;

	T squaredLength() const;
	T length() const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;

	bool nonZero() const;

	Vector3 normalized() const;

	Vector3 permuted(unsigned int i,
					 unsigned int j,
					 unsigned int k) const;

	void normalize();

	void reverse();
	
	void flipToSameHemisphereAs(const Vector3& other);
	void flipToSameHemisphereAs(const Normal3& normal);
};

// Vector3 typedefs

typedef Vector3<imp_float> Vector3F;
typedef Vector3<int> Vector3I;

// Functions on Vector3 objects

template <typename T>
inline Vector3<T> operator*(T factor, const Vector3<T>& vector)
{
	return vector*factor;
}

template <typename T>
inline Vector3<T> abs(const Vector3<T>& vector)
{
	return Vector3<T>(std::abs(vector.x), std::abs(vector.y), std::abs(vector.z));
}

// Component-wise minimum
template <typename T>
inline Vector3<T> min(const Vector3<T>& vector_1, const Vector3<T>& vector_2)
{
	return Vector3<T>(std::min(vector_1.x, vector_2.x),
					  std::min(vector_1.y, vector_2.y),
					  std::min(vector_1.z, vector_2.z));
}

// Component-wise maximum
template <typename T>
inline Vector3<T> max(const Vector3<T>& vector_1, const Vector3<T>& vector_2)
{
	return Vector3<T>(std::max(vector_1.x, vector_2.x),
					  std::max(vector_1.y, vector_2.y),
					  std::max(vector_1.z, vector_2.z));
}

// Creates right-handed coordinate system based on given normalized axis vector
template <typename T>
inline void coordinateSystem(const Vector3<T>& axis_1,
							 Vector3<T>* axis_2,
							 Vector3<T>* axis_3)
{
	*axis_2 = (std::abs(axis_1.x) >= std::abs(axis_1.y))?
			  Vector3<T>(-axis_1.z,		   0,  axis_1.x)/std::sqrt(axis_1.z*axis_1.z + axis_1.x*axis_1.x) :
			  Vector3<T>(		 0, axis_1.z, -axis_1.y)/std::sqrt(axis_1.z*axis_1.z + axis_1.y*axis_1.y);

	*axis_3 = axis_1.cross(*axis_2);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Vector3<T>& vector)
{
	stream << "[" << vector.x << ", " << vector.y <<  ", " << vector.z << "]";
	return stream;
}

// Vector3 method implementations

template <typename T>
inline Vector3<T>::Vector3()
	: x(0), y(0), z(0)
{}

template <typename T>
inline Vector3<T>::Vector3(T x, T y, T z)
	: x(x), y(y), z(z)
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline Vector3<T>::Vector3(const Normal3<T>& normal)
	: x(normal.x), y(normal.y), z(normal.z)
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline bool Vector3<T>::hasNaNs() const
{
	return std::isnan(x) || std::isnan(y) || std::isnan(z);
}

template <typename T>
inline T Vector3<T>::operator[](unsigned int dimension) const
{
	imp_assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline T& Vector3<T>::operator[](unsigned int dimension)
{
	imp_assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator+(const Vector3& other) const
{
	return Vector3(x + other.x, y + other.y, z + other.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator-(const Vector3& other) const
{
	return Vector3(x - other.x, y - other.y, z - other.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator*(T factor) const
{
	return Vector3(x*factor, y*factor, z*factor);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator/(T divisor) const
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	return Vector3(x*factor, y*factor, z*factor);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator-() const
{
	return Vector3(-x, -y, -z);
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator+=(const Vector3& other)
{
	x += other.x; y += other.y; z += other.z;
	return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator-=(const Vector3& other)
{
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator*=(T factor)
{
	x *= factor; y *= factor; z *= factor;
	return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator/=(T divisor)
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	x *= factor; y *= factor; z *= factor;
	return *this;
}

template <typename T>
inline T Vector3<T>::dot(const Vector3& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

template <typename T>
inline T Vector3<T>::dot(const Normal3<T>& normal) const
{
	return x*normal.x + y*normal.y + z*normal.z;
}

// Absoulte value of dot product
template <typename T>
inline T Vector3<T>::absDot(const Vector3& other) const
{
	return std::abs(dot(other));
}

// Absoulte value of dot product
template <typename T>
inline T Vector3<T>::absDot(const Normal3<T>& normal) const
{
	return std::abs(dot(normal));
}

template <typename T>
inline Vector3<T> Vector3<T>::cross(const Vector3& other) const
{
	// Convert components to double precision before subtracting
	// in order to avoid catastrophic cancellation
	double x1 = (double)(x);
	double y1 = (double)(y);
	double z1 = (double)(z);
	double x2 = (double)(other.x);
	double y2 = (double)(other.y);
	double z2 = (double)(other.z);

	return Vector3(static_cast<T>(y1*z2 - z1*y2),
				   static_cast<T>(z1*x2 - x1*z2),
			 	   static_cast<T>(x1*y2 - y1*x2));
}

template <typename T>
inline T Vector3<T>::squaredLength() const
{
	return x*x + y*y + z*z;
}

template <typename T>
inline T Vector3<T>::length() const
{
	return std::sqrt(squaredLength());
}

template <typename T>
inline T Vector3<T>::minComponent() const
{
	return std::min(x, std::min(y, z));
}

template <typename T>
inline T Vector3<T>::maxComponent() const
{
	return std::max(x, std::max(y, z));
}

template <typename T>
inline unsigned int Vector3<T>::minDimension() const
{
	return (x <= y)? ((x <= z)? 0 : 2) : ((y <= z)? 1 : 2);
}

template <typename T>
inline unsigned int Vector3<T>::maxDimension() const
{
	return (x >= y)? ((x >= z)? 0 : 2) : ((y >= z)? 1 : 2);
}

template <typename T>
inline bool Vector3<T>::nonZero() const
{
	return x != 0 || y != 0 || z != 0;
}

template <typename T>
inline Vector3<T> Vector3<T>::normalized() const
{
	return (*this)/length();
}

template <typename T>
inline Vector3<T> Vector3<T>::permuted(unsigned int i,
									   unsigned int j,
									   unsigned int k) const
{
	return Vector3(operator[](i), operator[](j), operator[](k));
}

template <typename T>
inline void Vector3<T>::normalize()
{
	*this /= length();
}

template <typename T>
inline void Vector3<T>::reverse()
{
	x = -x; y = -y; z = -z;
}

template <typename T>
inline void Vector3<T>::flipToSameHemisphereAs(const Vector3& other)
{
	if (dot(other) < 0.0f)
		reverse();
}

template <typename T>
inline void Vector3<T>::flipToSameHemisphereAs(const Normal3<T>& normal)
{
	if (dot(normal) < 0.0f)
		reverse();
}

} // RayImpact
} // Impact
