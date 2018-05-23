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
                 const std::shared_ptr< Texture<imp_float> >& mixing_ratio);

    T evaluate(const SurfaceScatteringEvent& scattering_event) const;

	std::string toString() const;
};

// MixedTexture function declarations

Texture<imp_float>* createMixedFloatTexture(const Transformation& texture_to_world,
                                            const TextureParameterSet& parameters);

Texture<Spectrum>* createMixedSpectrumTexture(const Transformation& texture_to_world,
                                              const TextureParameterSet& parameters);

// MixedTexture inline method definitions

template <typename T>
inline MixedTexture<T>::MixedTexture(const std::shared_ptr< Texture<T> >& texture_1,
									 const std::shared_ptr< Texture<T> >& texture_2,
									 const std::shared_ptr< Texture<imp_float> >& mixing_ratio)
    : texture_1(texture_1), texture_2(texture_2), mixing_ratio(mixing_ratio)
{}

template <typename T>
inline T MixedTexture<T>::evaluate(const SurfaceScatteringEvent& scattering_event) const
{
    imp_float texture_2_weight = mixing_ratio->evaluate(scattering_event);

    return (1 - texture_2_weight)*texture_1->evaluate(scattering_event) + texture_2_weight*texture_2->evaluate(scattering_event);
}

template <typename T>
inline std::string MixedTexture<T>::toString() const
{
    std::ostringstream stream;
	stream << "{texture 1: " << *texture_1 << ", texture 2: " << *texture_2 << ", ratio: " << *mixing_ratio << "}";
    return stream.str();
}

} // RayImpact
} // Impact
