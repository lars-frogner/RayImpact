#pragma once
#include "Material.hpp"
#include "Spectrum.hpp"
#include "Texture.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// GlassMaterial declarations

class GlassMaterial : public Material {

private:

    std::shared_ptr< Texture<ReflectionSpectrum> > reflectance_texture; // Texture with reflection spectra (colors) of the material
    std::shared_ptr< Texture<TransmissionSpectrum> > transmittance_texture; // Texture with transmittance spectra (colors) of the material
    std::shared_ptr< Texture<imp_float> > roughness_u_texture; // Texture with values controlling the roughness of the material in the parameteric u-direction
    std::shared_ptr< Texture<imp_float> > roughness_v_texture; // Texture with values controlling the roughness of the material in the parameteric v-direction
    std::shared_ptr< Texture<imp_float> > refractive_index_texture; // Texture with values controlling the refractive index of the material
    std::shared_ptr< Texture<imp_float> > bump_map; // Bump map for the material
	const bool normalized_roughness; // Whether the roughness values are in the range [0, 1] (otherwise they correspond directly to slope deviations)

public:

    GlassMaterial(const std::shared_ptr< Texture<ReflectionSpectrum> >& reflectance_texture,
                  const std::shared_ptr< Texture<TransmissionSpectrum> >& transmittance_texture,
                  const std::shared_ptr< Texture<imp_float> >& roughness_u_texture,
                  const std::shared_ptr< Texture<imp_float> >& roughness_v_texture,
                  const std::shared_ptr< Texture<imp_float> >& refractive_index_texture,
                  const std::shared_ptr< Texture<imp_float> >& bump_map,
                  bool normalized_roughness);

    void generateBSDF(SurfaceScatteringEvent* scattering_event,
                      RegionAllocator& allocator,
                      TransportMode transport_mode,
                      bool allow_multiple_scattering_types) const;
};

// GlassMaterial function declarations

Material* createGlassMaterial(const TextureParameterSet& parameters);

// GlassMaterial inline method definitions

inline GlassMaterial::GlassMaterial(const std::shared_ptr< Texture<ReflectionSpectrum> >& reflectance_texture,
									const std::shared_ptr< Texture<TransmissionSpectrum> >& transmittance_texture,
									const std::shared_ptr< Texture<imp_float> >& roughness_u_texture,
									const std::shared_ptr< Texture<imp_float> >& roughness_v_texture,
									const std::shared_ptr< Texture<imp_float> >& refractive_index_texture,
									const std::shared_ptr< Texture<imp_float> >& bump_map,
									bool normalized_roughness)
    : reflectance_texture(reflectance_texture),
	  transmittance_texture(transmittance_texture),
      roughness_u_texture(roughness_u_texture),
      roughness_v_texture(roughness_v_texture),
      refractive_index_texture(refractive_index_texture),
      bump_map(bump_map),
	  normalized_roughness(normalized_roughness)
{}

} // RayImpact
} // Impact
