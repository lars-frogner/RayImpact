#pragma once
#include "Texture.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// MixedTexture declarations

template <typename T>
class MixedTexture : public Texture<T> {

private:

    std::shared_ptr< Texture<T> > texture_1; // First texture to mix
    std::shared_ptr< Texture<T> > texture_2; // Second texture to mix
    std::shared_ptr< Texture<imp_float> > mixing_ratio; // Texture of mixing ratios for interpolating between the two textures

public:

    MixedTexture(const std::shared_ptr< Texture<T> >& texture_1,
                 const std::shared_ptr< Texture<T> >& texture_2,
                 const std::shared_ptr< Texture<imp_float> >& mixing_ratio)
        : texture_1(texture_1), texture_2(texture_2), mixing_ratio(mixing_ratio)
    {}

    T evaluate(const SurfaceScatteringEvent& scattering_event) const
    {
        imp_float texture_2_weight = mixing_ratio->evaluate(scattering_event);

        return (1 - texture_2_weight)*texture_1->evaluate(scattering_event) + texture_2_weight*texture_2->evaluate(scattering_event);
    }
};

// MixedTexture creation

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
