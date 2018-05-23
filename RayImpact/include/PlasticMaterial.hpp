#pragma once
#include "Material.hpp"
#include "Spectrum.hpp"
#include "Texture.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// PlasticMaterial declarations

class PlasticMaterial : public Material {

private:

    std::shared_ptr< Texture<ReflectionSpectrum> > diffuse_reflectance_texture; // Texture with reflection spectra (colors) for the diffuse component of the material
    std::shared_ptr< Texture<ReflectionSpectrum> > glossy_reflectance_texture; // Texture with reflection spectra (colors) for the glossy component of the material
    std::shared_ptr< Texture<imp_float> > roughness_texture; // Texture with values controlling the roughness of the material
    std::shared_ptr< Texture<imp_float> > bump_map; // Bump map for the material
    const bool normalized_roughness; // Whether the roughness values are in the range [0, 1] (otherwise they correspond directly to slope deviations)

public:

    PlasticMaterial(const std::shared_ptr< Texture<ReflectionSpectrum> >& diffuse_reflectance_texture,
                    const std::shared_ptr< Texture<ReflectionSpectrum> >& glossy_reflectance_texture,
                    const std::shared_ptr< Texture<imp_float> >& roughness_texture,
                    const std::shared_ptr< Texture<imp_float> >& bump_map,
                    bool normalized_roughness);

    void generateBSDF(SurfaceScatteringEvent* scattering_event,
                      RegionAllocator& allocator,
                      TransportMode transport_mode,
                      bool allow_multiple_scattering_types) const;
};

// PlasticMaterial function declarations

Material* createPlasticMaterial(const TextureParameterSet& parameters);

// PlasticMaterial inline method definitions

inline PlasticMaterial::PlasticMaterial(const std::shared_ptr< Texture<ReflectionSpectrum> >& diffuse_reflectance_texture,
										const std::shared_ptr< Texture<ReflectionSpectrum> >& glossy_reflectance_texture,
										const std::shared_ptr< Texture<imp_float> >& roughness_texture,
										const std::shared_ptr< Texture<imp_float> >& bump_map,
										bool normalized_roughness)
    : diffuse_reflectance_texture(diffuse_reflectance_texture),
      glossy_reflectance_texture(glossy_reflectance_texture),
      roughness_texture(roughness_texture),
      bump_map(bump_map),
      normalized_roughness(normalized_roughness)
{}

} // RayImpact
} // Impact
