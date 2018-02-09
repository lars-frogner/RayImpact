#pragma once
#include "precision.hpp"
#include "error.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include "Ray.hpp"
#include <algorithm>
#include <ostream>
#include <limits>

namespace Impact {
namespace RayImpact {

// Constant used in ray-box intersection testing
constexpr imp_float maxDistanceSafetyFactor = 1 + 2*errorPowerBound(3);

// BoundingBox declarations

template <typename T>
class BoundingBox {

public:
    Point3<T> lower_corner; // Corner with the lowest coordinate values
    Point3<T> upper_corner; // Corner with the highest coordinate values

    BoundingBox();

    explicit BoundingBox(const Point3<T>& lower_corner,
                         const Point3<T>& upper_corner);

    explicit BoundingBox(const Point3<T>& point);

    static BoundingBox aroundPoints(const Point3<T>& point_1,
                                    const Point3<T>& point_2);

    const Point3<T>& operator[](unsigned int point_idx) const;
    Point3<T>& operator[](unsigned int point_idx);

    Point3<T> corner(unsigned int corner_idx) const;

    bool overlaps(const BoundingBox& other) const;

    bool contains(const Point3<T>& point) const;
    bool containsExclusive(const Point3<T>& point) const;

    template <typename U>
    BoundingBox expanded(U amount) const;

    Vector3<T> diagonal() const;

    T surfaceArea() const;
    T volume() const;

    unsigned int maxDimension() const;

    Vector3<T> getLocalCoordinate(const Point3<T>& global_coord) const;
    Point3<T> getGlobalCoordinate(const Vector3<T>& local_coord) const;

    void boundingSphere(Point3<T>* center, T* radius) const;

    void enclose(const Point3<T>& point);

    bool hasIntersection(const Ray& ray,
                         imp_float* first_intersection_distance,
                         imp_float* second_intersection_distance) const;

    bool hasIntersection(const Ray& ray,
                         const Vector3F& inverse_direction) const;
};

// BoundingBox typedefs

typedef BoundingBox<imp_float> BoundingBoxF;
typedef BoundingBox<int> BoundingBoxI;

// Functions on BoundingBox objects

// Creates bounding box encompassing the given bounding box and point
template <typename T>
inline BoundingBox<T> unionOf(const BoundingBox<T>& bounding_box,
                              const Point3<T>& point)
{
    return BoundingBox<T>(min(bounding_box.lower_corner, point),
                          max(bounding_box.upper_corner, point));
}

// Creates bounding box encompassing both the given bounding boxes
template <typename T>
inline BoundingBox<T> unionOf(const BoundingBox<T>& bounding_box_1,
                              const BoundingBox<T>& bounding_box_2)
{
    return BoundingBox<T>(min(bounding_box_1.lower_corner, bounding_box_2.lower_corner),
                          max(bounding_box_1.upper_corner, bounding_box_2.upper_corner));
}

// Creates bounding box contained inside both the given bounding boxes
template <typename T>
inline BoundingBox<T> intersectionOf(const BoundingBox<T>& bounding_box_1,
                                     const BoundingBox<T>& bounding_box_2)
{
    return BoundingBox<T>(max(bounding_box_1.lower_corner, bounding_box_2.lower_corner),
                          min(bounding_box_1.upper_corner, bounding_box_2.upper_corner));
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const BoundingBox<T>& box)
{
    stream << "{lower corner = " << box.lower_corner << ", upper corner = " << box.upper_corner << "}";
    return stream;
}

// BoundingBox method implementations

template <typename T>
inline BoundingBox<T>::BoundingBox()
    : lower_corner(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max()),
      upper_corner(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest())
{}

template <typename T>
inline BoundingBox<T>::BoundingBox(const Point3<T>& lower_corner,
                                   const Point3<T>& upper_corner)
    : lower_corner(lower_corner),
      upper_corner(upper_corner)
{
    imp_assert(upper_corner >= lower_corner);
}

template <typename T>
inline BoundingBox<T>::BoundingBox(const Point3<T>& point)
    : lower_corner(point),
      upper_corner(point)
{}

template <typename T>
inline BoundingBox<T> BoundingBox<T>::aroundPoints(const Point3<T>& point_1,
                                                   const Point3<T>& point_2)
{
    return BoundingBox(min(point_1, point_2), max(point_1, point_2));
}

template <typename T>
inline const Point3<T>& BoundingBox<T>::operator[](unsigned int point_idx) const
{
    imp_assert(point_idx < 2);
    return (point_idx == 0)? lower_corner : upper_corner;
}

template <typename T>
inline Point3<T>& BoundingBox<T>::operator[](unsigned int point_idx)
{
    imp_assert(point_idx < 2);
    return (point_idx == 0)? lower_corner : upper_corner;
}

// Returns point corresponding to one of the corners
template <typename T>
inline Point3<T> BoundingBox<T>::corner(unsigned int corner_idx) const
{
    imp_assert(corner_idx < 8);
    return Point3<T>(operator[]( corner_idx & 1        ).x,  // lower_corner.x for 0, 2, 4, 6 and upper_corner.x for 1, 3, 5, 7
                     operator[]((corner_idx & 2)? 1 : 0).y,  // lower_corner.y for 0, 1, 4, 5 and upper_corner.y for 2, 3, 6, 7
                     operator[]((corner_idx & 4)? 1 : 0).z); // lower_corner.z for 0, 1, 2, 3 and upper_corner.z for 4, 5, 6, 7
}

template <typename T>
inline bool BoundingBox<T>::overlaps(const BoundingBox& other) const
{
    return (upper_corner.x >= other.lower_corner.x && lower_corner.x <= other.upper_corner.x) &&
           (upper_corner.y >= other.lower_corner.y && lower_corner.y <= other.upper_corner.y) &&
           (upper_corner.z >= other.lower_corner.z && lower_corner.z <= other.upper_corner.z);
}

template <typename T>
inline bool BoundingBox<T>::contains(const Point3<T>& point) const
{
    return (point.x >= lower_corner.x && point.x <= upper_corner.x) &&
           (point.y >= lower_corner.y && point.y <= upper_corner.y) &&
           (point.z >= lower_corner.z && point.z <= upper_corner.z);
}

template <typename T>
inline bool BoundingBox<T>::containsExclusive(const Point3<T>& point) const
{
    // Coordinates on the upper boundaries are not considered to be inside the box
    return (point.x >= lower_corner.x && point.x < upper_corner.x) &&
           (point.y >= lower_corner.y && point.y < upper_corner.y) &&
           (point.z >= lower_corner.z && point.z < upper_corner.z);
}

// Returns bounding box expanded uniformly by twice the given amount
template <typename T>
template <typename U>
inline BoundingBox<T> BoundingBox<T>::expanded(U amount) const
{
    Vector3<T> expansion(static_cast<T>(amount), static_cast<T>(amount), static_cast<T>(amount));

    return BoundingBox(lower_corner - expansion,
                       upper_corner + expansion);
}

// Returns vector going from the lower corner to the upper corner
template <typename T>
inline Vector3<T> BoundingBox<T>::diagonal() const
{
    return upper_corner - lower_corner;
}

template <typename T>
inline T BoundingBox<T>::surfaceArea() const
{
    const Vector3<T>& diagonal = diagonal();
    return 2*(diagonal.x*diagonal.y + diagonal.y*diagonal.z + diagonal.z*diagonal.x);
}

template <typename T>
inline T BoundingBox<T>::volume() const
{
    const Vector3<T>& diagonal = diagonal();
    return diagonal.x*diagonal.y*diagonal.z;
}

// Returns the dimension with largest extent
template <typename T>
inline unsigned int BoundingBox<T>::maxDimension() const
{
    const Vector3<T>& diagonal = diagonal();
    return (diagonal.x >= diagonal.y)? ((diagonal.x >= diagonal.z)? 0 : 2) : ((diagonal.y >= diagonal.z)? 1 : 2);
}

// Returns the coordinate of the given point relative to the bounding box,
// with zero at the lower corner and one at the upper corner
template <typename T>
inline Vector3<T> BoundingBox<T>::getLocalCoordinate(const Point3<T>& global_coord) const
{
    Vector3<T>& local_coord = global_coord - lower_corner;

    if (upper_corner.x > lower_corner.x)
        local_coord.x /= upper_corner.x - lower_corner.x;

    if (upper_corner.y > lower_corner.y)
        local_coord.y /= upper_corner.y - lower_corner.y;

    if (upper_corner.z > lower_corner.z)
        local_coord.z /= upper_corner.z - lower_corner.z;

    return local_coord;
}

// Returns the world-space coordinate corresponding a given local coordinate
template <typename T>
inline Point3<T> BoundingBox<T>::getGlobalCoordinate(const Vector3<T>& local_coord) const
{
    return Point3<T>(::Impact::lerp(lower_corner.x, upper_corner.x, local_coord.x),
                     ::Impact::lerp(lower_corner.y, upper_corner.y, local_coord.y),
                     ::Impact::lerp(lower_corner.z, upper_corner.z, local_coord.z));
}

// Gives the center and radius of a sphere encompassing the bounding box
template <typename T>
inline void BoundingBox<T>::boundingSphere(Point3<T>* center, T* radius) const
{
    *center = 0.5f*(lower_corner + upper_corner);
    *radius = (contains(*center))? distanceBetween(*center, upper_corner) : 0;
}

// Expands the bounding box to contain the given point
template <typename T>
inline void BoundingBox<T>::enclose(const Point3<T>& point)
{
    lower_corner.x = std::min(lower_corner.x, point.x);
    lower_corner.y = std::min(lower_corner.y, point.y);
    lower_corner.z = std::min(lower_corner.z, point.z);

    upper_corner.x = std::max(upper_corner.x, point.x);
    upper_corner.y = std::max(upper_corner.y, point.y);
    upper_corner.z = std::max(upper_corner.z, point.z);
}

template <typename T>
inline bool BoundingBox<T>::hasIntersection(const Ray& ray,
                                            imp_float* first_intersection_distance,
                                            imp_float* second_intersection_distance) const
{
    imp_float min_dist_temp, max_dist_temp;

    imp_float min_dist = 0;
    imp_float max_dist = ray.max_distance;

    imp_float inverse_ray_direction_x = 1.0f/ray.direction.x;

    if (inverse_ray_direction_x >= 0)
    {
        min_dist_temp = (lower_corner.x - ray.origin.x)*inverse_ray_direction_x;
        max_dist_temp = (upper_corner.x - ray.origin.x)*inverse_ray_direction_x;
    }
    else
    {
        min_dist_temp = (upper_corner.x - ray.origin.x)*inverse_ray_direction_x;
        max_dist_temp = (lower_corner.x - ray.origin.x)*inverse_ray_direction_x;
    }

    max_dist_temp *= maxDistanceSafetyFactor;

    if (min_dist_temp > min_dist)
        min_dist = min_dist_temp;

    if (max_dist_temp < max_dist)
        max_dist = max_dist_temp;

    if (min_dist > max_dist)
        return false;

    imp_float inverse_ray_direction_y = 1.0f/ray.direction.y;

    if (inverse_ray_direction_y >= 0)
    {
        min_dist_temp = (lower_corner.y - ray.origin.y)*inverse_ray_direction_y;
        max_dist_temp = (upper_corner.y - ray.origin.y)*inverse_ray_direction_y;
    }
    else
    {
        min_dist_temp = (upper_corner.y - ray.origin.y)*inverse_ray_direction_y;
        max_dist_temp = (lower_corner.y - ray.origin.y)*inverse_ray_direction_y;
    }

    max_dist_temp *= maxDistanceSafetyFactor;

    if (min_dist_temp > min_dist)
        min_dist = min_dist_temp;

    if (max_dist_temp < max_dist)
        max_dist = max_dist_temp;

    if (min_dist > max_dist)
        return false;

    imp_float inverse_ray_direction_z = 1.0f/ray.direction.z;

    if (inverse_ray_direction_z >= 0)
    {
        min_dist_temp = (lower_corner.z - ray.origin.z)*inverse_ray_direction_z;
        max_dist_temp = (upper_corner.z - ray.origin.z)*inverse_ray_direction_z;
    }
    else
    {
        min_dist_temp = (upper_corner.z - ray.origin.z)*inverse_ray_direction_z;
        max_dist_temp = (lower_corner.z - ray.origin.z)*inverse_ray_direction_z;
    }

    max_dist_temp *= maxDistanceSafetyFactor;

    if (min_dist_temp > min_dist)
        min_dist = min_dist_temp;

    if (max_dist_temp < max_dist)
        max_dist = max_dist_temp;

    if (min_dist > max_dist)
        return false;

    if (first_intersection_distance) *first_intersection_distance = min_dist;
    if (second_intersection_distance) *second_intersection_distance = max_dist;

    return true;
}

template <typename T>
inline bool BoundingBox<T>::hasIntersection(const Ray& ray,
                                            const Vector3F& inverse_direction) const
{
    imp_float min_dist, max_dist, min_dist_temp, max_dist_temp;

    if (inverse_direction.x >= 0)
    {
        min_dist = (lower_corner.x - ray.origin.x)*inverse_direction.x;
        max_dist = (upper_corner.x - ray.origin.x)*inverse_direction.x;
    }
    else
    {
        min_dist = (upper_corner.x - ray.origin.x)*inverse_direction.x;
        max_dist = (lower_corner.x - ray.origin.x)*inverse_direction.x;
    }

    if (inverse_direction.y >= 0)
    {
        min_dist_temp = (lower_corner.y - ray.origin.y)*inverse_direction.y;
        max_dist_temp = (upper_corner.y - ray.origin.y)*inverse_direction.y;
    }
    else
    {
        min_dist_temp = (upper_corner.y - ray.origin.y)*inverse_direction.y;
        max_dist_temp = (lower_corner.y - ray.origin.y)*inverse_direction.y;
    }

    max_dist *= maxDistanceSafetyFactor;
    max_dist_temp *= maxDistanceSafetyFactor;

    if (min_dist > max_dist_temp || min_dist_temp > max_dist)
        return false;

    if (min_dist_temp > min_dist)
        min_dist = min_dist_temp;

    if (max_dist_temp < max_dist)
        max_dist = max_dist_temp;

    if (inverse_direction.z >= 0)
    {
        min_dist_temp = (lower_corner.z - ray.origin.z)*inverse_direction.z;
        max_dist_temp = (upper_corner.z - ray.origin.z)*inverse_direction.z;
    }
    else
    {
        min_dist_temp = (upper_corner.z - ray.origin.z)*inverse_direction.z;
        max_dist_temp = (lower_corner.z - ray.origin.z)*inverse_direction.z;
    }

    max_dist_temp *= maxDistanceSafetyFactor;

    if (min_dist > max_dist_temp || min_dist_temp > max_dist)
        return false;

    if (min_dist_temp > min_dist)
        min_dist = min_dist_temp;

    if (max_dist_temp < max_dist)
        max_dist = max_dist_temp;

    return (min_dist < ray.max_distance && max_dist > 0);
}

} // RayImpact
} // Impact
