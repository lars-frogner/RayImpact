#include "BilinearInterpolationTexture.hpp"

namespace Impact {
namespace RayImpact {

// BilinearInterpolationTexture function definitions

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
