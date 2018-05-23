#pragma once
#include "ConstantTexture.hpp"
#include "ParameterSet.hpp"

namespace Impact {
namespace RayImpact {

// ConstantTexture function definitions

Texture<imp_float>* createConstantFloatTexture(const Transformation& texture_to_world,
                                               const TextureParameterSet& parameters)
{
    imp_float value = parameters.getSingleFloatValue("value", 0.0f);

    return new ConstantTexture<imp_float>(value);
}

Texture<Spectrum>* createConstantSpectrumTexture(const Transformation& texture_to_world,
                                                 const TextureParameterSet& parameters)
{
    const Spectrum& value = parameters.getSingleSpectrumValue("value", Spectrum(0.0f));

    return new ConstantTexture<Spectrum>(value);
}

} // RayImpact
} // Impact
