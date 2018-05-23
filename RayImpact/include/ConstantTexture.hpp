#pragma once
#include "Texture.hpp"

namespace Impact {
namespace RayImpact {

// ConstantTexture declarations

template <typename T>
class ConstantTexture : public Texture<T> {

private:
    const T value; // Value that the texture evaluates to

public:

    ConstantTexture(const T& value);

    T evaluate(const SurfaceScatteringEvent& scattering_event) const;

	std::string toString() const;
};

// ConstantTexture function declarations

Texture<imp_float>* createConstantFloatTexture(const Transformation& texture_to_world,
                                               const TextureParameterSet& parameters);

Texture<Spectrum>* createConstantSpectrumTexture(const Transformation& texture_to_world,
                                                 const TextureParameterSet& parameters);

// ConstantTexture inline method definitions

template <typename T>
inline ConstantTexture<T>::ConstantTexture(const T& value)
    : value(value)
{}

template <typename T>
inline T ConstantTexture<T>::evaluate(const SurfaceScatteringEvent& scattering_event) const
{
    return value;
}

template <typename T>
inline std::string ConstantTexture<T>::toString() const
{
    std::ostringstream stream;
	stream << "{value: " << value << "}";
    return stream.str();
}

} // RayImpact
} // Impact
