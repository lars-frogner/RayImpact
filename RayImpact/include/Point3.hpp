#pragma once
#include "precision.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <ostream>

namespace Impact {
namespace RayImpact {

// Forward declarations

template <typename T>
class Vector3;

// Point3 declarations

template <typename T>
class Point3 {

private:
	
	bool hasNaNs() const;

public:

	T x, y, z; // Coordinate values

	Point3();
	Point3(T x, T y, T z);

	template <typename U>
	explicit Point3(const Point3<U>& other);

	template <typename U>
	explicit operator Vector3<U>() const;

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);
	
	Point3<T> operator+(const Vector3<T>& vector) const;
	Point3<T> operator-(const Vector3<T>& vector) const;

	Point3<T>& operator+=(const Vector3<T>& vector);
	Point3<T>& operator-=(const Vector3<T>& vector);
	
	Vector3<T> operator-(const Point3<T>& other) const;
	
	// Addition of points and multiplication of points with scalars
	// is only geometrically correct if the weights sum to one.
	Point3<T> operator+(const Point3<T>& other) const;
	Point3<T> operator*(T weight) const;

	bool operator>=(const Point3<T>& other) const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;

	Point3<T> permuted(unsigned int i,
					   unsigned int j,
					   unsigned int k) const;
};

// Point3 typedefs

typedef Point3<imp_float> Point3F;
typedef Point3<int> Point3I;

// Functions on Point3 objects

template <typename T>
inline Point3<T> operator*(T weight, const Point3<T>& point)
{
	return point*weight;
}

template <typename T>
inline Point3<T> abs(const Point3<T>& point)
{
	return Point3<T>(std::abs(point.x), std::abs(point.y), std::abs(point.z));
}

template <typename T>
inline Point3<T> floor(const Point3<T>& point)
{
	return Point3<T>(std::floor(point.x), std::floor(point.y), std::floor(point.z));
}

template <typename T>
inline Point3<T> ceil(const Point3<T>& point)
{
	return Point3<T>(std::ceil(point.x), std::ceil(point.y), std::ceil(point.z));
}

// Component-wise minimum
template <typename T>
inline Point3<T> min(const Point3<T>& point_1, const Point3<T>& point_2)
{
	return Point3<T>(std::min(point_1.x, point_2.x),
					 std::min(point_1.y, point_2.y),
					 std::min(point_1.z, point_2.z));
}

// Component-wise maximum
template <typename T>
inline Point3<T> max(const Point3<T>& point_1, const Point3<T>& point_2)
{
	return Point3<T>(std::max(point_1.x, point_2.x),
					 std::max(point_1.y, point_2.y),
					 std::max(point_1.z, point_2.z));
}

template <typename T>
inline T squaredDistanceBetween(const Point3<T>& point_1, const Point3<T>& point_2)
{
	return (point_2 - point_1).squaredLength();
}

template <typename T>
inline T distanceBetween(const Point3<T>& point_1, const Point3<T>& point_2)
{
	return (point_2 - point_1).length();
}

// Linear interpolation between two points
template <typename T>
inline Point3<T> lerp(const Point3<T>& point_1, const Point3<T>& point_2, T weight)
{
	return point_1*(1 - weight) + point_2*weight;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Point3<T>& point)
{
	stream << "(" << point.x << ", " << point.y <<  ", " << point.z << ")";
	return stream;
}

// Point3 method implementations

template <typename T>
inline Point3<T>::Point3()
	: x(0), y(0), z(0)
{}

template <typename T>
inline Point3<T>::Point3(T x, T y, T z)
	: x(x), y(y), z(z)
{
	assert(!hasNaNs());
}

template<typename T>
template<typename U>
inline Point3<T>::Point3(const Point3<U>& other)
	: x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z))
{
	assert(!hasNaNs());
}

template <typename T>
inline bool Point3<T>::hasNaNs() const
{
	return std::isnan(x) || std::isnan(y) || std::isnan(z);
}

template<typename T>
template<typename U>
inline Point3<T>::operator Vector3<U>() const
{
	return Vector3<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
}

template <typename T>
inline T Point3<T>::operator[](unsigned int dimension) const
{
	assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline T& Point3<T>::operator[](unsigned int dimension)
{
	assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline Point3<T> Point3<T>::operator+(const Vector3<T>& vector) const
{
	return Point3<T>(x + vector.x, y + vector.y, z + vector.z);
}

template <typename T>
inline Point3<T> Point3<T>::operator-(const Vector3<T>& vector) const
{
	return Point3<T>(x - vector.x, y - vector.y, z - vector.z);
}

template <typename T>
inline Point3<T>& Point3<T>::operator+=(const Vector3<T>& vector)
{
	x += vector.x; y += vector.y; z += vector.z;
	return *this;
}

template <typename T>
inline Point3<T>& Point3<T>::operator-=(const Vector3<T>& vector)
{
	x -= vector.x; y -= vector.y; z -= vector.z;
	return *this;
}

template <typename T>
inline Vector3<T> Point3<T>::operator-(const Point3<T>& other) const
{
	return Vector3<T>(x - other.x, y - other.y, z - other.z);
}

template <typename T>
inline Point3<T> Point3<T>::operator+(const Point3<T>& other) const
{
	return Point3<T>(x + other.x, y + other.y, z + other.z);
}

template <typename T>
inline Point3<T> Point3<T>::operator*(T weight) const
{
	return Point3<T>(x*weight, y*weight, z*weight);
}

template <typename T>
inline bool Point3<T>::operator>=(const Point3<T>& other) const
{
	return x >= other.x && y >= other.y && z >= other.z;
}

template <typename T>
inline T Point3<T>::minComponent() const
{
	return std::min(x, std::min(y, z));
}

template <typename T>
inline T Point3<T>::maxComponent() const
{
	return std::max(x, std::max(y, z));
}

template <typename T>
inline unsigned int Point3<T>::minDimension() const
{
	return (x <= y)? ((x <= z)? 0 : 2) : ((y <= z)? 1 : 2);
}

template <typename T>
inline unsigned int Point3<T>::maxDimension() const
{
	return (x >= y)? ((x >= z)? 0 : 2) : ((y >= z)? 1 : 2);
}

template <typename T>
inline Point3<T> Point3<T>::permuted(unsigned int i,
									 unsigned int j,
									 unsigned int k) const
{
	return Point3<T>(operator[](i), operator[](j), operator[](k));
}

} // RayImpact
} // Impact
