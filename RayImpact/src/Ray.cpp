#include "Ray.hpp"

namespace Impact {
namespace RayImpact {

// Ray method implementations

Ray::Ray()
    : origin(),
      direction(),
      max_distance(IMP_INFINITY),
      time(0),
      medium(nullptr)
{}

Ray::Ray(const Point3F& origin,
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

Point3F Ray::operator()(imp_float distance) const
{
    return origin + direction*distance;
}

// RayWithOffsets method implementations

RayWithOffsets::RayWithOffsets()
    : Ray::Ray(),
      has_offsets(false)
{}

RayWithOffsets::RayWithOffsets(const Point3F& origin,
                               const Vector3F& direction,
                               imp_float max_distance /* = IMP_INFINITY */,
                               imp_float time /* = 0 */,
                               const Medium* medium /* = nullptr */)
    : Ray::Ray(origin, direction, max_distance, time, medium),
      has_offsets(false)
{}

RayWithOffsets::RayWithOffsets(const Ray& ray)
    : Ray::Ray(ray),
      has_offsets(false)
{}

void RayWithOffsets::scaleOffsets(imp_float scale)
{
    x_offset_ray_origin = origin + (x_offset_ray_origin - origin)*scale;
    x_offset_ray_direction = direction + (x_offset_ray_direction - direction)*scale;

    y_offset_ray_origin = origin + (y_offset_ray_origin - origin)*scale;
    y_offset_ray_direction = direction + (y_offset_ray_direction - direction)*scale;
}

// Printing functions

std::ostream& operator<<(std::ostream& stream, const Ray& ray)
{
    stream << "{origin = " << ray.origin << ", direction = " << ray.direction << ", max = " << ray.max_distance << ", time = " << ray.time << "}";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const RayWithOffsets& ray)
{
    stream << "{origin = " << ray.origin << ", direction = " << ray.direction << ", max = " << ray.max_distance << ", time = " << ray.time << "}";
    return stream;
}

} // RayImpact
} // Impact
