#pragma once
#include "Texture.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// ScaledTexture declarations

template <typename T_scale, typename T>
class ScaledTexture : public Texture<T> {

private:

    std::shared_ptr< Texture<T_scale> > scale; // Texture of scaling values to apply to the other texture
    std::shared_ptr< Texture<T> > texture; // Texture to scale

public:

    ScaledTexture(const std::shared_ptr< Texture<T_scale> >& scale,
                  const std::shared_ptr< Texture<T> >& texture)
        : scale(scale), texture(texture)
    {}

    T evaluate(const SurfaceScatteringEvent& scattering_event) const
    {
        return scale->evaluate(scattering_event)*texture->evaluate(scattering_event);
    }
};

// ScaledTexture creation

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
