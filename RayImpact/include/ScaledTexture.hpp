#pragma once
#include "Texture.hpp"
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

} // RayImpact
} // Impact
