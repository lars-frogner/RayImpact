#pragma once
#include "Texture.hpp"
#include "ParameterSet.hpp"

namespace Impact {
namespace RayImpact {

// ConstantTexture declarations

template <typename T>
class ConstantTexture : public Texture<T> {

private:
    const T value; // Value that the texture evaluates to

public:

    ConstantTexture(const T& value)
        : value(value)
    {}

    T evaluate(const SurfaceScatteringEvent& scattering_event) const
    {
        return value;
    }
};

// ConstantTexture creation

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
