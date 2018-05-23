#include "MixedTexture.hpp"

namespace Impact {
namespace RayImpact {

// MixedTexture function definitions

Texture<imp_float>* createMixedFloatTexture(const Transformation& texture_to_world,
                                            const TextureParameterSet& parameters)
{
    return new MixedTexture<imp_float>(parameters.getFloatTexture("texture_1", 0.0f),
                                       parameters.getFloatTexture("texture_2", 0.0f),
                                       parameters.getFloatTexture("mixing_ratio", 0.0f));
}

Texture<Spectrum>* createMixedSpectrumTexture(const Transformation& texture_to_world,
                                              const TextureParameterSet& parameters)
{
    return new MixedTexture<Spectrum>(parameters.getSpectrumTexture("texture_1", Spectrum(0.0f)),
                                      parameters.getSpectrumTexture("texture_2", Spectrum(0.0f)),
                                      parameters.getFloatTexture("mixing_ratio", 0.0f));
}

} // RayImpact
} // Impact
