#pragma once
#include "Texture.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// BilinearInterpolationTexture declarations

template <typename T>
class BilinearInterpolationTexture : public Texture<T> {

private:

    const T value_00; // Value for s = 0, t = 0
    const T value_01; // Value for s = 0, t = 1
    const T value_10; // Value for s = 1, t = 0
    const T value_11; // Value for s = 1, t = 1

    std::unique_ptr<TextureMapper2D> mapper; // Texture mapper to use for determining texture coordinates

public:

    BilinearInterpolationTexture(const T& value_00, const T& value_01,
                                 const T& value_10, const T& value_11,
                                 std::unique_ptr<TextureMapper2D> texture_mapper)
        : value_00(value_00), value_01(value_01),
          value_10(value_10), value_11(value_11),
          mapper(std::move(texture_mapper))
    {}

    T evaluate(const SurfaceScatteringEvent& scattering_event) const
    {
        Vector2F dstdx, dstdy;

        const Point2F& coord = mapper->textureCoordinate(scattering_event, &dstdx, &dstdy);

        return (1 - coord.x)*(1 - coord.y)*value_00 +
               (1 - coord.x)*     coord.y *value_01 +
                    coord.x *(1 - coord.y)*value_10 +
                    coord.x *     coord.y *value_11;
    }
};

} // RayImpact
} // Impact
