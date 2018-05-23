#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include <ostream>
#include <sstream>
#include <string>

namespace Impact {
namespace RayImpact {

// Ray declarations

class Ray {

public:

    Point3F origin; // Ray origin
    Vector3F direction; // Ray direction (generally not normalized)
    mutable imp_float max_distance; // Distance from ray origin to ray endpoint
    imp_float time; // Point in time associated with the ray
    const Medium* medium; // Medium associated with ray

    Ray();

    Ray(const Point3F& origin,
        const Vector3F& direction,
        imp_float max_distance = IMP_INFINITY,
        imp_float time = 0,
        const Medium* medium = nullptr);

    Point3F operator()(imp_float distance) const;

	std::string toString() const;
};

// RayWithOffsets declarations

class RayWithOffsets : public Ray {

public:

    bool has_offsets;

    Point3F x_offset_ray_origin; // Origin of x-offset ray
    Vector3F x_offset_ray_direction; // Direction of x-offset ray

    Point3F y_offset_ray_origin; // Origin of y-offset ray
    Vector3F y_offset_ray_direction; // Direction of y-offset ray

    RayWithOffsets();

    RayWithOffsets(const Point3F& origin,
                   const Vector3F& direction,
                   imp_float max_distance = IMP_INFINITY,
                   imp_float time = 0,
                   const Medium* medium = nullptr);

    RayWithOffsets(const Ray& ray);

    void scaleOffsets(imp_float scale);

	std::string toString() const;
};

// Ray inline method definitions

inline Ray::Ray()
    : origin(),
      direction(),
      max_distance(IMP_INFINITY),
      time(0),
      medium(nullptr)
{}

inline Ray::Ray(const Point3F& origin,
				const Vector3F& direction,
				imp_float max_distance /* = IMP_INFINITY */,
				imp_float time /* = 0 */,
				const Medium* medium /* = nullptr */)
    : origin(origin),
      direction(direction),
      max_distance(max_distance),
      time(time),
      medium(medium)
{}

inline Point3F Ray::operator()(imp_float distance) const
{
    return origin + direction*distance;
}

inline std::string Ray::toString() const
{
    std::ostringstream stream;
	stream << "{origin: " << origin << ", direction: " << direction << ", max: " << max_distance << ", time: " << time << "}";
    return stream.str();
}

// RayWithOffsets inline method definitions

inline RayWithOffsets::RayWithOffsets()
    : Ray::Ray(),
      has_offsets(false)
{}

inline RayWithOffsets::RayWithOffsets(const Point3F& origin,
									  const Vector3F& direction,
									  imp_float max_distance /* = IMP_INFINITY */,
									  imp_float time /* = 0 */,
									  const Medium* medium /* = nullptr */)
    : Ray::Ray(origin, direction, max_distance, time, medium),
      has_offsets(false)
{}

inline RayWithOffsets::RayWithOffsets(const Ray& ray)
    : Ray::Ray(ray),
      has_offsets(false)
{}

inline std::string RayWithOffsets::toString() const
{
    std::ostringstream stream;
	stream << "{origin: " << origin << ", direction: " << direction << ", max: " << max_distance << ", time: " << time << "}";
    return stream.str();
}

// Ray inline function definitions

inline std::ostream& operator<<(std::ostream& stream, const Ray& ray)
{
	stream << ray.toString();
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const RayWithOffsets& ray)
{
    stream << ray.toString();
    return stream;
}

} // RayImpact
} // Impact
