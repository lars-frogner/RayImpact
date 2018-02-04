#pragma once
#include "Material.hpp"
#include "Spectrum.hpp"
#include "Texture.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// MixedMaterial declarations

class MixedMaterial : public Material {

private:

    std::shared_ptr<Material> material_1; // First material to mix
    std::shared_ptr<Material> material_2; // Second material to mix
    std::shared_ptr< Texture<Spectrum> > mixing_ratio_texture; // Texture with values controlling the mixing ratio of the materials

public:

    MixedMaterial(const std::shared_ptr<Material>& material_1,
                  const std::shared_ptr<Material>& material_2,
                  const std::shared_ptr< Texture<Spectrum> >& mixing_ratio_texture);

    void generateBSDF(SurfaceScatteringEvent* scattering_event,
                      RegionAllocator& allocator,
                      TransportMode transport_mode,
                      bool allow_multiple_scattering_types) const;
};

} // RayImpact
} // Impact
