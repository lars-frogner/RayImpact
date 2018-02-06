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

    ConstantTexture(const T& value)
        : value(value)
    {}

    T evaluate(const SurfaceScatteringEvent& scattering_event) const
    {
        return value;
    }
};

} // RayImpact
} // Impact
