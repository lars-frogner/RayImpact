#pragma once
#include "precision.hpp"
#include "error.hpp"
#include "math.hpp"
#include <cmath>
#include <algorithm>
#include <ostream>

namespace Impact {
namespace RayImpact {

// Forward declarations
	
template <typename T>
class Vector2;

template <typename T>
class Vector3;

template <typename T>
class Normal3;

template <typename T>
class Point2;

template <typename T>
class Point3;

// Typedefs

typedef Vector2<imp_float> Vector2F;
typedef Vector2<int> Vector2I;

typedef Vector3<imp_float> Vector3F;
typedef Vector3<int> Vector3I;

typedef Normal3<imp_float> Normal3F;

typedef Point2<imp_float> Point2F;
typedef Point2<int> Point2I;

typedef Point3<imp_float> Point3F;
typedef Point3<int> Point3I;

// Vector2 declarations

template <typename T>
class Vector2 {

private:
	
	bool hasNaNs() const;

public:

	T x, y; // Component values

	Vector2();
	explicit Vector2(T x, T y);

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);

	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator*(T factor) const;
	Vector2 operator/(T divisor) const;
	
	Vector2 operator-() const;

	Vector2& operator+=(const Vector2& other);
	Vector2& operator-=(const Vector2& other);
	Vector2& operator*=(T factor);
	Vector2& operator/=(T divisor);

	T dot(const Vector2& other) const;
	T absDot(const Vector2& other) const;

	T squaredLength() const;
	T length() const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;

	Vector2 normalized() const;

	void normalize();
};

// Vector3 declarations

template <typename T>
class Vector3 {

private:
	
	bool hasNaNs() const;

public:

	T x, y, z; // Component values

	Vector3();
	explicit Vector3(T x, T y, T z);

	explicit Vector3(const Normal3<T>& normal);

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
	T dot(const Normal3<T>& normal) const;

	T absDot(const Vector3& other) const;
	T absDot(const Normal3<T>& normal) const;
	
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
	void flipToSameHemisphereAs(const Normal3<T>& normal);
};

// Normal3 declarations

template <typename T>
class Normal3 {

private:
	
	bool hasNaNs() const;

public:

	T x, y, z; // Component values

	Normal3();
	explicit Normal3(T x, T y, T z);

	explicit Normal3(const Vector3<T>& vector);

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);

	Normal3 operator+(const Normal3& other) const;
	Normal3 operator-(const Normal3& other) const;
	Normal3 operator*(T factor) const;
	Normal3 operator/(T divisor) const;
	
	Normal3 operator-() const;

	Normal3& operator+=(const Normal3& other);
	Normal3& operator-=(const Normal3& other);
	Normal3& operator*=(T factor);
	Normal3& operator/=(T divisor);

	T dot(const Normal3& other) const;
	T dot(const Vector3<T>& vector) const;

	T absDot(const Normal3& other) const;
	T absDot(const Vector3<T>& vector) const;

	T squaredLength() const;
	T length() const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;

	bool nonZero() const;

	Normal3 normalized() const;

	Normal3 permuted(unsigned int i,
					 unsigned int j,
					 unsigned int k) const;

	void normalize();

	void reverse();

	void flipToSameHemisphereAs(const Normal3& other);
	void flipToSameHemisphereAs(const Vector3<T>& vector);
};

// Point2 declarations

template <typename T>
class Point2 {

private:
	
	bool hasNaNs() const;

public:

	T x, y; // Coordinate values

	Point2();
	explicit Point2(T x, T y);

	explicit Point2(const Point3<T>& point3);

	template <typename U>
	explicit Point2(const Point2<U>& other);

	template <typename U>
	explicit operator Vector2<U>() const;

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);
	
	Point2 operator+(const Vector2<T>& vector) const;
	Point2 operator-(const Vector2<T>& vector) const;

	Point2& operator+=(const Vector2<T>& vector);
	Point2& operator-=(const Vector2<T>& vector);
	
	Vector2<T> operator-(const Point2& other) const;
	
	// Addition of points and multiplication of points with scalars
	// is only geometrically correct if the weights sum to one.
	Point2 operator+(const Point2& other) const;
	Point2 operator*(T weight) const;
	
	bool operator==(const Point2& other) const;
	bool operator!=(const Point2& other) const;

	bool operator>=(const Point2& other) const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;
};

// Point3 declarations

template <typename T>
class Point3 {

private:
	
	bool hasNaNs() const;

public:

	T x, y, z; // Coordinate values

	Point3();
	explicit Point3(T x, T y, T z);

	template <typename U>
	explicit Point3(const Point3<U>& other);

	template <typename U>
	explicit operator Vector3<U>() const;

	T operator[](unsigned int dimension) const;
	T& operator[](unsigned int dimension);
	
	Point3 operator+(const Vector3<T>& vector) const;
	Point3 operator-(const Vector3<T>& vector) const;

	Point3& operator+=(const Vector3<T>& vector);
	Point3& operator-=(const Vector3<T>& vector);
	
	Vector3<T> operator-(const Point3& other) const;
	
	// Addition of points and multiplication of points with scalars
	// is only geometrically correct if the weights sum to one.
	Point3 operator+(const Point3& other) const;
	Point3 operator*(T weight) const;

	bool operator>=(const Point3& other) const;

	T minComponent() const;
	T maxComponent() const;

	unsigned int minDimension() const;
	unsigned int maxDimension() const;

	Point3 permuted(unsigned int i,
					unsigned int j,
					unsigned int k) const;
};

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
	return isNaN(x) || isNaN(y);
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
inline Vector2<T> Vector2<T>::operator+(const Vector2& other) const
{
	return Vector2(x + other.x, y + other.y);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator-(const Vector2& other) const
{
	return Vector2(x - other.x, y - other.y);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator*(T factor) const
{
	return Vector2(x*factor, y*factor);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator/(T divisor) const
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	return Vector2(x*factor, y*factor);
}

template <typename T>
inline Vector2<T> Vector2<T>::operator-() const
{
	return Vector2(-x, -y);
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator+=(const Vector2& other)
{
	x += other.x; y += other.y;
	return *this;
}

template <typename T>
inline Vector2<T>& Vector2<T>::operator-=(const Vector2& other)
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
inline T Vector2<T>::dot(const Vector2& other) const
{
	return x*other.x + y*other.y;
}

// Absoulte value of dot product
template <typename T>
inline T Vector2<T>::absDot(const Vector2& other) const
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
	return isNaN(x) || isNaN(y) || isNaN(z);
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
	return isNaN(x) || isNaN(y) || isNaN(z);
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
inline Normal3<T> Normal3<T>::operator+(const Normal3& other) const
{
	return Normal3(x + other.x, y + other.y, z + other.z);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator-(const Normal3& other) const
{
	return Normal3(x - other.x, y - other.y, z - other.z);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator*(T factor) const
{
	return Normal3(x*factor, y*factor, z*factor);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator/(T divisor) const
{
	imp_assert(divisor != 0);

	imp_float factor = 1.0f/divisor;
	return Normal3(x*factor, y*factor, z*factor);
}

template <typename T>
inline Normal3<T> Normal3<T>::operator-() const
{
	return Normal3(-x, -y, -z);
}

template <typename T>
inline Normal3<T>& Normal3<T>::operator+=(const Normal3& other)
{
	x += other.x; y += other.y; z += other.z;
	return *this;
}

template <typename T>
inline Normal3<T>& Normal3<T>::operator-=(const Normal3& other)
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
inline T Normal3<T>::dot(const Normal3& other) const
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
inline T Normal3<T>::absDot(const Normal3& other) const
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
	return Normal3(operator[](i), operator[](j), operator[](k));
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
inline void Normal3<T>::flipToSameHemisphereAs(const Normal3& other)
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

template<typename T>
template<typename U>
inline Point2<T>::Point2(const Point2<U>& other)
	: x(static_cast<T>(other.x)), y(static_cast<T>(other.y))
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline Point2<T>::Point2(const Point3<T>& point3)
	: x(point3.x), y(point3.y)
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline bool Point2<T>::hasNaNs() const
{
	return isNaN(x) || isNaN(y);
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
	return Point2(x + vector.x, y + vector.y);
}

template <typename T>
inline Point2<T> Point2<T>::operator-(const Vector2<T>& vector) const
{
	return Point2(x - vector.x, y - vector.y);
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
inline Vector2<T> Point2<T>::operator-(const Point2& other) const
{
	return Vector2<T>(x - other.x, y - other.y);
}

template <typename T>
inline Point2<T> Point2<T>::operator+(const Point2& other) const
{
	return Point2(x + other.x, y + other.y);
}

template <typename T>
inline Point2<T> Point2<T>::operator*(T weight) const
{
	return Point2(x*weight, y*weight);
}

template <typename T>
inline bool Point2<T>::operator==(const Point2& other) const
{
	return x == other.x && y == other.y;
}

template <typename T>
inline bool Point2<T>::operator!=(const Point2& other) const
{
	return !(*this == other);
}

template <typename T>
inline bool Point2<T>::operator>=(const Point2& other) const
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

// Point3 method implementations

template <typename T>
inline Point3<T>::Point3()
	: x(0), y(0), z(0)
{}

template <typename T>
inline Point3<T>::Point3(T x, T y, T z)
	: x(x), y(y), z(z)
{
	imp_assert(!hasNaNs());
}

template<typename T>
template<typename U>
inline Point3<T>::Point3(const Point3<U>& other)
	: x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z))
{
	imp_assert(!hasNaNs());
}

template <typename T>
inline bool Point3<T>::hasNaNs() const
{
	return isNaN(x) || isNaN(y) || isNaN(z);
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
	imp_assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline T& Point3<T>::operator[](unsigned int dimension)
{
	imp_assert(dimension < 3);
	return (dimension == 0)? x : ((dimension == 1)? y : z);
}

template <typename T>
inline Point3<T> Point3<T>::operator+(const Vector3<T>& vector) const
{
	return Point3(x + vector.x, y + vector.y, z + vector.z);
}

template <typename T>
inline Point3<T> Point3<T>::operator-(const Vector3<T>& vector) const
{
	return Point3(x - vector.x, y - vector.y, z - vector.z);
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
inline Vector3<T> Point3<T>::operator-(const Point3& other) const
{
	return Vector3<T>(x - other.x, y - other.y, z - other.z);
}

template <typename T>
inline Point3<T> Point3<T>::operator+(const Point3& other) const
{
	return Point3(x + other.x, y + other.y, z + other.z);
}

template <typename T>
inline Point3<T> Point3<T>::operator*(T weight) const
{
	return Point3(x*weight, y*weight, z*weight);
}

template <typename T>
inline bool Point3<T>::operator>=(const Point3& other) const
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
	return Point3(operator[](i), operator[](j), operator[](k));
}

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
inline Point2<T> min(const Point2<T>& point_1, const Point2<T>& point_2)
{
	return Point2<T>(std::min(point_1.x, point_2.x),
					 std::min(point_1.y, point_2.y));
}

// Component-wise maximum
template <typename T>
inline Point2<T> max(const Point2<T>& point_1, const Point2<T>& point_2)
{
	return Point2<T>(std::max(point_1.x, point_2.x),
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

} // RayImpact
} // Impact
