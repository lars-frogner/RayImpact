#pragma once
#include "precision.hpp"
#include "error.hpp"
#include <cmath>
#include <algorithm>
#include <ostream>

namespace Impact {
namespace RayImpact {

// Vector2 declarations

template <typename T>
class Vector2 {

private:
	
	bool hasNaNs() const;

public:

	T x, y; // Component values

	Vector2();
	Vector2(T x, T y);

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);

	Vector2<T> operator+(const Vector2<T>& other) const;
	Vector2<T> operator-(const Vector2<T>& other) const;
	Vector2<T> operator*(T factor) const;
	Vector2<T> operator/(T divisor) const;
	
	Vector2<T> operator-() const;

	Vector2<T>& operator+=(const Vector2<T>& other);
	Vector2<T>& operator-=(const Vector2<T>& other);
	Vector2<T>& operator*=(T factor);
	Vector2<T>& operator/=(T divisor);

	T dot(const Vector2<T>& other) const;
	T absDot(const Vector2<T>& other) const;

	T squaredLength() const;
	T length() const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;

	Vector2<T> normalized() const;

	void normalize();
};

// Vector2 typedefs

typedef Vector2<imp_float> Vector2F;
typedef Vector2<int> Vector2I;

// Functions on Vector2 objects

template <typename T>
inline Vector2<T> operator*(T factor, const Vector2<T>& vector)
{
	return vector*factor;
}

template <typename T>
inline Vector2<T> abs(const Vector2<T>& vector)
{
	return Vector2<T>(std::abs(vector.x), std::abs(vector.y));
}

// Component-wise minimum
template <typename T>
inline Vector2<T> min(const Vector2<T>& vector_1, const Vector2<T>& vector_2)
{
	return Vector2<T>(std::min(vector_1.x, vector_2.x),
					  std::min(vector_1.y, vector_2.y));
}

// Component-wise maximum
template <typename T>
inline Vector2<T> max(const Vector2<T>& vector_1, const Vector2<T>& vector_2)
{
	return Vector2<T>(std::max(vector_1.x, vector_2.x),
					  std::max(vector_1.y, vector_2.y));
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Vector2<T>& vector)
{
	stream << "[" << vector.x << ", " << vector.y << "]";
	return stream;
}

// Vector2 method implementations

template <typename T>
inline Vector2<T>::Vector2()
	: x(0), y(0)
{}

template <typename T>
inline Vector2<T>::Vector2(T x, T y)
	: x(x), y(y)
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline bool Vector2<T>::hasNaNs() const
{
	return std::isnan(x) || std::isnan(y);
}

template <typename T>
inline T Vector2<T>::operator[](unsigned int dimension) const
{
	imp_assert(dimension < 2);
	return (dimension == 0)? x : y;
}

template <typename T>
inline T& Vector2<T>::operator[](unsigned int dimension)
{
	imp_assert(dimension < 2);
	return (dimension == 0)? x : y;
}

template <typename T>
inline Vector2<T> Vector2<T>::operator+(const Vector2<T>& other) const
{
	return Vector2<T>(x + other.x, y + other.y);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const
{
	return Vector2<T>(x - other.x, y - other.y);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator*(T factor) const
{
	return Vector2<T>(x*factor, y*factor);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator/(T divisor) const
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	return Vector2<T>(x*factor, y*factor);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator-() const
{
	return Vector2<T>(-x, -y);
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& other)
{
	x += other.x; y += other.y;
	return *this;
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other)
{
	x -= other.x; y -= other.y;
	return *this;
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator*=(T factor)
{
	x *= factor; y *= factor;
	return *this;
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator/=(T divisor)
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	x *= factor; y *= factor;
	return *this;
}

template <typename T>
inline T Vector2<T>::dot(const Vector2<T>& other) const
{
	return x*other.x + y*other.y;
}

// Absoulte value of dot product
template <typename T>
inline T Vector2<T>::absDot(const Vector2<T>& other) const
{
	return std::abs(dot(other));
}

template <typename T>
inline T Vector2<T>::squaredLength() const
{
	return x*x + y*y;
}

template <typename T>
inline T Vector2<T>::length() const
{
	return std::sqrt(squaredLength());
}

template <typename T>
inline T Vector2<T>::minComponent() const
{
	return std::min(x, y);
}

template <typename T>
inline T Vector2<T>::maxComponent() const
{
	return std::max(x, y);
}

template <typename T>
inline unsigned int Vector2<T>::minDimension() const
{
	return (x <= y)? 0 : 1;
}

template <typename T>
inline unsigned int Vector2<T>::maxDimension() const
{
	return (x >= y)? 0 : 1;
}

template <typename T>
inline Vector2<T> Vector2<T>::normalized() const
{
	return (*this)/length();
}

template <typename T>
inline void Vector2<T>::normalize()
{
	*this /= length();
}

} // RayImpact
} // Impact
