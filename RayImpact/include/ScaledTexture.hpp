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
                  const std::shared_ptr< Texture<T> >& texture);

    T evaluate(const SurfaceScatteringEvent& scattering_event) const;

	std::string toString() const;
};

// ScaledTexture function declarations

ScaledTexture<imp_float, imp_float>* createScaledFloatTexture(const Transformation& texture_to_world,
                                                              const TextureParameterSet& parameters);

ScaledTexture<Spectrum, Spectrum>* createScaledSpectrumTexture(const Transformation& texture_to_world,
                                                               const TextureParameterSet& parameters);

// MixedTexture inline method definitions

template <typename T_scale, typename T>
inline ScaledTexture<T_scale, T>::ScaledTexture(const std::shared_ptr< Texture<T_scale> >& scale,
												const std::shared_ptr< Texture<T> >& texture)
    : scale(scale), texture(texture)
{}

template <typename T_scale, typename T>
inline T ScaledTexture<T_scale, T>::evaluate(const SurfaceScatteringEvent& scattering_event) const
{
    return scale->evaluate(scattering_event)*texture->evaluate(scattering_event);
}

template <typename T_scale, typename T>
inline std::string ScaledTexture<T_scale, T>::toString() const
{
    std::ostringstream stream;
	stream << "{scale: " << *scale << ", texture: " << *texture << "}";
    return stream.str();
}

} // RayImpact
} // Impact
