#include "Ray.hpp"

namespace Impact {
namespace RayImpact {

// RayWithOffsets method definitions

void RayWithOffsets::scaleOffsets(imp_float scale)
{
    x_offset_ray_origin = origin + (x_offset_ray_origin - origin)*scale;
    x_offset_ray_direction = direction + (x_offset_ray_direction - direction)*scale;

    y_offset_ray_origin = origin + (y_offset_ray_origin - origin)*scale;
    y_offset_ray_direction = direction + (y_offset_ray_direction - direction)*scale;
}

} // RayImpact
} // Impact
