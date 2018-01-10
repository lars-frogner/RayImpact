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
class Vector2;

template <typename T>
class Point3;

// Point2 declarations

template <typename T>
class Point2 {

private:
	
	bool hasNaNs() const;

public:

	T x, y; // Coordinate values

	Point2();
	Point2(T x, T y);

	explicit Point2(const Point3<T>& point3);

	template <typename U>
	explicit Point2(const Point2<U>& other);

	template <typename U>
	explicit operator Vector2<U>() const;

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);
	
	Point2<T> operator+(const Vector2<T>& vector) const;
	Point2<T> operator-(const Vector2<T>& vector) const;

	Point2<T>& operator+=(const Vector2<T>& vector);
	Point2<T>& operator-=(const Vector2<T>& vector);
	
	Vector2<T> operator-(const Point2<T>& other) const;
	
	// Addition of points and multiplication of points with scalars
	// is only geometrically correct if the weights sum to one.
	Point2<T> operator+(const Point2<T>& other) const;
	Point2<T> operator*(T weight) const;

	bool operator>=(const Point2<T>& other) const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;
};

// Point2 typedefs

typedef Point2<imp_float> Point2F;
typedef Point2<int> Point2I;

// Functions on Point2 objects

template <typename T>
inline Point2<T> operator*(T weight, const Point2<T>& point)
{
	return point*weight;
}

template <typename T>
inline Point2<T> abs(const Point2<T>& point)
{
	return Point2<T>(std::abs(point.x), std::abs(point.y));
}

template <typename T>
inline Point2<T> floor(const Point2<T>& point)
{
	return Point2<T>(std::floor(point.x), std::floor(point.y));
}

template <typename T>
inline Point2<T> ceil(const Point2<T>& point)
{
	return Point2<T>(std::ceil(point.x), std::ceil(point.y));
}

// Component-wise minimum
template <typename T>
inline Vector2<T> min(const Point2<T>& point_1, const Point2<T>& point_2)
{
	return Vector2<T>(std::min(point_1.x, point_2.x),
					  std::min(point_1.y, point_2.y));
}

// Component-wise maximum
template <typename T>
inline Vector2<T> max(const Point2<T>& point_1, const Point2<T>& point_2)
{
	return Vector2<T>(std::max(point_1.x, point_2.x),
					  std::max(point_1.y, point_2.y));
}

template <typename T>
inline T squaredDistanceBetween(const Point2<T>& point_1, const Point2<T>& point_2)
{
	return (point_2 - point_1).squaredLength();
}

template <typename T>
inline T distanceBetween(const Point2<T>& point_1, const Point2<T>& point_2)
{
	return (point_2 - point_1).length();
}

// Linear interpolation between two points
template <typename T>
inline Point2<T> lerp(const Point2<T>& point_1, const Point2<T>& point_2, T weight)
{
	return point_1*(1 - weight) + point_2*weight;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Point2<T>& point)
{
	stream << "(" << point.x << ", " << point.y << ")";
	return stream;
}

// Point2 method implementations

template <typename T>
inline Point2<T>::Point2()
	: x(0), y(0)
{}

template <typename T>
inline Point2<T>::Point2(T x, T y)
	: x(x), y(y)
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline Point2<T>::Point2(const Point3<T>& point3)
	: x(point3.x), y(point3.y)
{
	imp_assert(!hasNaNs());
}

template<typename T>
template<typename U>
inline Point2<T>::Point2(const Point2<U>& other)
	: x(static_cast<T>(other.x)), y(static_cast<T>(other.y))
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline bool Point2<T>::hasNaNs() const
{
	return std::isnan(x) || std::isnan(y);
}

template<typename T>
template<typename U>
inline Point2<T>::operator Vector2<U>() const
{
	return Vector2<U>(static_cast<U>(x), static_cast<U>(y));
}

template <typename T>
inline T Point2<T>::operator[](unsigned int dimension) const
{
	imp_assert(dimension < 2);
	return (dimension == 0)? x : y;
}

template <typename T>
inline T& Point2<T>::operator[](unsigned int dimension)
{
	imp_assert(dimension < 2);
	return (dimension == 0)? x : y;
}

template <typename T>
inline Point2<T> Point2<T>::operator+(const Vector2<T>& vector) const
{
	return Point2<T>(x + vector.x, y + vector.y);
}

template <typename T>
inline Point2<T> Point2<T>::operator-(const Vector2<T>& vector) const
{
	return Point2<T>(x - vector.x, y - vector.y);
}

template <typename T>
inline Point2<T>& Point2<T>::operator+=(const Vector2<T>& vector)
{
	x += vector.x; y += vector.y;
	return *this;
}

template <typename T>
inline Point2<T>& Point2<T>::operator-=(const Vector2<T>& vector)
{
	x -= vector.x; y -= vector.y;
	return *this;
}

template <typename T>
inline Vector2<T> Point2<T>::operator-(const Point2<T>& other) const
{
	return Vector2<T>(x - other.x, y - other.y);
}

template <typename T>
inline Point2<T> Point2<T>::operator+(const Point2<T>& other) const
{
	return Point2<T>(x + other.x, y + other.y);
}

template <typename T>
inline Point2<T> Point2<T>::operator*(T weight) const
{
	return Point2<T>(x*weight, y*weight);
}

template <typename T>
inline bool Point2<T>::operator>=(const Point2<T>& other) const
{
	return x >= other.x && y >= other.y;
}

template <typename T>
inline T Point2<T>::minComponent() const
{
	return std::min(x, y);
}

template <typename T>
inline T Point2<T>::maxComponent() const
{
	return std::max(x, y);
}

template <typename T>
inline unsigned int Point2<T>::minDimension() const
{
	return (x <= y)? 0 : 1;
}

template <typename T>
inline unsigned int Point2<T>::maxDimension() const
{
	return (x >= y)? 0 : 1;
}

} // RayImpact
} // Impact
