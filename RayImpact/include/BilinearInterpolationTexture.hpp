#pragma once
#include "Texture.hpp"
#include "error.hpp"
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

// BilinearInterpolationTexture creation

Texture<imp_float>* createBilinearInterpolationFloatTexture(const Transformation& texture_to_world,
                                                            const TextureParameterSet& parameters)
{
    imp_float value_00 = parameters.getSingleFloatValue("value_00", 0.0f);
    imp_float value_01 = parameters.getSingleFloatValue("value_01", 0.0f);
    imp_float value_10 = parameters.getSingleFloatValue("value_10", 0.0f);
    imp_float value_11 = parameters.getSingleFloatValue("value_11", 0.0f);

    std::unique_ptr<TextureMapper2D> mapper;

    create_2D_texture_mapper(parameters, mapper);

    return new BilinearInterpolationTexture<imp_float>(value_00, value_01,
                                                       value_10, value_11,
                                                       std::move(mapper));
}

Texture<Spectrum>* createBilinearInterpolationSpectrumTexture(const Transformation& texture_to_world,
                                                              const TextureParameterSet& parameters)
{
    const Spectrum& value_00 = parameters.getSingleSpectrumValue("value_00", Spectrum(0.0f));
    const Spectrum& value_01 = parameters.getSingleSpectrumValue("value_01", Spectrum(0.0f));
    const Spectrum& value_10 = parameters.getSingleSpectrumValue("value_10", Spectrum(0.0f));
    const Spectrum& value_11 = parameters.getSingleSpectrumValue("value_11", Spectrum(0.0f));

    std::unique_ptr<TextureMapper2D> mapper;

    create_2D_texture_mapper(parameters, mapper);

    return new BilinearInterpolationTexture<Spectrum>(value_00, value_01,
                                                      value_10, value_11,
                                                      std::move(mapper));
}

} // RayImpact
} // Impact
