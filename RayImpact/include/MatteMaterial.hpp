#pragma once
#include "Material.hpp"
#include "Spectrum.hpp"
#include "Texture.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// MatteMaterial declarations

class MatteMaterial : public Material {

private:

    std::shared_ptr< Texture<ReflectionSpectrum> > reflectance_texture; // Texture with reflection spectra (colors) of the material
    std::shared_ptr< Texture<imp_float> > slope_deviation_texture; // Texture with values controlling the roughness of the material
    std::shared_ptr< Texture<imp_float> > bump_map; // Bump map for the material

public:

    MatteMaterial(const std::shared_ptr< Texture<ReflectionSpectrum> >& reflectance_texture,
                  const std::shared_ptr< Texture<imp_float> >& slope_deviation_texture,
                  const std::shared_ptr< Texture<imp_float> >& bump_map);

    void generateBSDF(SurfaceScatteringEvent* scattering_event,
                      RegionAllocator& allocator,
                      TransportMode transport_mode,
                      bool allow_multiple_scattering_types) const;
};

// MatteMaterial creation

Material* createMatteMaterial(const TextureParameterSet& parameters);

} // RayImpact
} // Impact
