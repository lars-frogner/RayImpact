#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "Vector2.hpp"
#include "Point2.hpp"
#include <cassert>
#include <algorithm>
#include <ostream>

namespace Impact {
namespace RayImpact {

// BoundingRectangle declarations

template <typename T>
class BoundingRectangle {

public:
	Point2<T> lower_corner; // Corner with the lowest coordinate values
	Point2<T> upper_corner; // Corner with the highest coordinate values

	BoundingRectangle();
	
	BoundingRectangle(const Point2<T>& lower_corner,
					  const Point2<T>& upper_corner);
	
	BoundingRectangle(const Point2<T>& point);

	static BoundingRectangle<T> aroundPoints(const Point2<T>& point_1,
											 const Point2<T>& point_2);

	const Point2<T>& operator[](unsigned int point_idx) const;
	Point2<T>& operator[](unsigned int point_idx);

	Point2<T> corner(unsigned int corner_idx) const;

	bool overlaps(const BoundingRectangle<T>& other) const;

	bool contains(const Point2<T>& point) const;
	bool containsExclusive(const Point2<T>& point) const;

	template <typename U>
	BoundingRectangle<T> expanded(U amount) const;

	Vector2<T> diagonal() const;

	T area() const;

	unsigned int maxDimension() const;
	
	Vector2<T> getLocalCoordinate(const Point2<T>& global_coord) const;
	Point2<T> getGlobalCoordinate(const Vector2<T>& local_coord) const;
	
	void enclose(const Point2<T>& point);
};

// BoundingRectangle typedefs

typedef BoundingRectangle<imp_float> BoundingRectangleF;
typedef BoundingRectangle<int> BoundingRectangleI;

// Functions on BoundingRectangle objects

// Creates bounding rectangle encompassing the given bounding rectangle and point
template <typename T>
inline BoundingRectangle<T> unionOf(const BoundingRectangle<T>& bounding_rectangle,
									const Point2<T>& point)
{
	return BoundingRectangle<T>(min(bounding_rectangle.lower_corner, point),
								max(bounding_rectangle.upper_corner, point));
}

// Creates bounding rectangle encompassing both the given bounding rectangles
template <typename T>
inline BoundingRectangle<T> unionOf(const BoundingRectangle<T>& bounding_rectangle_1,
									const BoundingRectangle<T>& bounding_rectangle_2)
{
	return BoundingRectangle<T>(min(bounding_rectangle_1.lower_corner, bounding_rectangle_2.lower_corner),
								max(bounding_rectangle_1.upper_corner, bounding_rectangle_2.upper_corner));
}

// Creates bounding rectangle contained inside both the given bounding rectangles
template <typename T>
inline BoundingRectangle<T> intersectionOf(const BoundingRectangle<T>& bounding_rectangle_1,
										   const BoundingRectangle<T>& bounding_rectangle_2)
{
	return BoundingRectangle<T>(max(bounding_rectangle_1.lower_corner.x, bounding_rectangle_2.lower_corner.x),
								min(bounding_rectangle_1.upper_corner.x, bounding_rectangle_2.upper_corner.x));
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const BoundingRectangle<T>& rectangle)
{
	stream << "{lower corner = " << rectangle.lower_corner << ", upper corner = " << rectangle.upper_corner << "}";
	return stream;
}

// BoundingRectangle method implementations

template <typename T>
inline BoundingRectangle<T>::BoundingRectangle()
	: lower_corner(IMP_MAX, IMP_MAX),
	  upper_corner(IMP_LOWEST, IMP_LOWEST)
{}

template <typename T>
inline BoundingRectangle<T>::BoundingRectangle(const Point2<T>& lower_corner,
											   const Point2<T>& upper_corner)
	: lower_corner(lower_corner),
	  upper_corner(upper_corner)
{
	assert(upper_corner >= lower_corner);
}

template <typename T>
inline BoundingRectangle<T>::BoundingRectangle(const Point2<T>& point)
	: lower_corner(point),
	  upper_corner(point)
{}

template <typename T>
inline BoundingRectangle<T> BoundingRectangle<T>::aroundPoints(const Point2<T>& point_1,
															   const Point2<T>& point_2)
{
	return BoundingRectangle<T>(min(point_1, point_2), max(point_1, point_2));
}

template <typename T>
inline const Point2<T>& BoundingRectangle<T>::operator[](unsigned int point_idx) const
{
	assert(point_idx < 2);
	return (point_idx == 0)? lower_corner : upper_corner;
}

template <typename T>
inline Point2<T>& BoundingRectangle<T>::operator[](unsigned int point_idx)
{
	assert(point_idx < 2);
	return (point_idx == 0)? lower_corner : upper_corner;
}

// Returns point corresponding to one of the corners
template <typename T>
inline Point2<T> BoundingRectangle<T>::corner(unsigned int corner_idx) const
{
	assert(corner_idx < 4);
	return Point2<T>(operator[]( corner_idx & 1        ).x,  // lower_corner.x for 0, 2 and upper_corner.x for 1, 3
					 operator[]((corner_idx & 2)? 1 : 0).y); // lower_corner.y for 0, 1 and upper_corner.y for 2, 3
}

template <typename T>
inline bool BoundingRectangle<T>::overlaps(const BoundingRectangle<T>& other) const
{
	return (upper_corner.x >= other.lower_corner.x && lower_corner.x <= other.upper_corner.x) &&
		   (upper_corner.y >= other.lower_corner.y && lower_corner.y <= other.upper_corner.y);
}

template <typename T>
inline bool BoundingRectangle<T>::contains(const Point2<T>& point) const
{
	return (point.x >= lower_corner.x && point.x <= upper_corner.x) &&
		   (point.y >= lower_corner.y && point.y <= upper_corner.y);
}

template <typename T>
inline bool BoundingRectangle<T>::containsExclusive(const Point2<T>& point) const
{
	// Coordinates on the upper boundaries are not considered to be inside the rectangle
	return (point.x >= lower_corner.x && point.x < upper_corner.x) &&
		   (point.y >= lower_corner.y && point.y < upper_corner.y);
}

// Returns bounding rectangle expanded uniformly by twice the given amount 
template <typename T>
template <typename U>
inline BoundingRectangle<T> BoundingRectangle<T>::expanded(U amount) const
{
	Vector2<T> expansion(static_cast<T>(amount), static_cast<T>(amount));

	return BoundingRectangle<T>(lower_corner - expansion,
								upper_corner + expansion);
}

// Returns vector going from the lower corner to the upper corner
template <typename T>
inline Vector2<T> BoundingRectangle<T>::diagonal() const
{
	return upper_corner - lower_corner;
}

template <typename T>
inline T BoundingRectangle<T>::area() const
{
	const Vector2<T>& diagonal = diagonal();
	return diagonal.x*diagonal.y;
}

// Returns the dimension with largest extent
template <typename T>
inline unsigned int BoundingRectangle<T>::maxDimension() const
{
	const Vector2<T>& diagonal = diagonal();
	return (diagonal.x >= diagonal.y)? 0 : 1;
}

// Returns the coordinate of the given point relative to the bounding rectangle,
// with zero at the lower corner and one at the upper corner
template <typename T>
inline Vector2<T> BoundingRectangle<T>::getLocalCoordinate(const Point2<T>& global_coord) const
{
	Vector2<T>& local_coord = global_coord - lower_corner;

	if (upper_corner.x > lower_corner.x)
		local_coord.x /= upper_corner.x - lower_corner.x;

	if (upper_corner.y > lower_corner.y)
		local_coord.y /= upper_corner.y - lower_corner.y;

	return local_coord;
}

// Returns the world-space coordinate corresponding a given local coordinate
template <typename T>
inline Point2<T> BoundingRectangle<T>::getGlobalCoordinate(const Vector2<T>& local_coord) const
{
	return Point2<T>(lerp(lower_corner.x, upper_corner.x, local_coord.x),
					 lerp(lower_corner.y, upper_corner.y, local_coord.y));
}

// Expands the bounding rectangle to contain the given point
template <typename T>
void BoundingRectangle<T>::enclose(const Point2<T>& point)
{
	lower_corner.x = std::min(lower_corner.x, point.x);
	lower_corner.y = std::min(lower_corner.y, point.y);

	upper_corner.x = std::max(upper_corner.x, point.x);
	upper_corner.y = std::max(upper_corner.y, point.y);
}

} // RayImpact
} // Impact
