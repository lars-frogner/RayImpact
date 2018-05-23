#include "ScaledTexture.hpp"

namespace Impact {
namespace RayImpact {

// ScaledTexture function definitions

ScaledTexture<imp_float, imp_float>* createScaledFloatTexture(const Transformation& texture_to_world,
                                                              const TextureParameterSet& parameters)
{
    return new ScaledTexture<imp_float, imp_float>(parameters.getFloatTexture("scale", 0.0f),
                                                   parameters.getFloatTexture("texture", 0.0f));
}

ScaledTexture<Spectrum, Spectrum>* createScaledSpectrumTexture(const Transformation& texture_to_world,
                                                               const TextureParameterSet& parameters)
{
    return new ScaledTexture<Spectrum, Spectrum>(parameters.getSpectrumTexture("scale", Spectrum(0.0f)),
                                                 parameters.getSpectrumTexture("texture", Spectrum(0.0f)));
}

} // RayImpact
} // Impact
