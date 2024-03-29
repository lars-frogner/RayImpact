#include "MixedMaterial.hpp"
#include "memory.hpp"
#include "math.hpp"
#include "BSDF.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// MixedMaterial method definitions

void MixedMaterial::generateBSDF(SurfaceScatteringEvent* scattering_event,
                                 RegionAllocator& allocator,
                                 TransportMode transport_mode,
                                 bool allow_multiple_scattering_types) const
{
    const Spectrum& material_2_weight = mixing_ratio_texture->evaluate(*scattering_event).clamped();
    const Spectrum& material_1_weight = (Spectrum(1.0f) - material_2_weight).clamped();

    SurfaceScatteringEvent scattering_event_copy(*scattering_event);

    material_1->generateBSDF(scattering_event, allocator, transport_mode, allow_multiple_scattering_types);
    material_2->generateBSDF(&scattering_event_copy, allocator, transport_mode, allow_multiple_scattering_types);

    unsigned int n_components_1 = scattering_event->bsdf->numberOfComponents();
    unsigned int n_components_2 = scattering_event_copy.bsdf->numberOfComponents();

    for (unsigned int i = 0; i < n_components_1; i++)
        scattering_event->bsdf->bxdfs[i] = allocated_in_region(allocator, ScaledBXDF)(scattering_event->bsdf->bxdfs[i],
                                                                                      material_1_weight);

    for (unsigned int i = 0; i < n_components_2; i++)
        scattering_event->bsdf->addComponent(allocated_in_region(allocator, ScaledBXDF)(scattering_event_copy.bsdf->bxdfs[i],
                                                                                        material_2_weight));
}

// MixedMaterial function definitions

Material* createMixedMaterial(const std::shared_ptr<Material>& material_1,
                              const std::shared_ptr<Material>& material_2,
                              const TextureParameterSet& parameters)
{
	const std::shared_ptr< Texture<Spectrum> >& mixing_ratio_texture = parameters.getSpectrumTexture("mixing_ratio", Spectrum(0.0f));
    
	if (RIMP_OPTIONS.verbosity >= IMP_MATERIALS_VERBOSITY)
	{
		printInfoMessage("Material:"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Type:", "Mixed",
						 "Mixing ratio:", mixing_ratio_texture->toString().c_str());
	}

	return new MixedMaterial(material_1, material_2,
                             mixing_ratio_texture);
}

} // RayImpact
} // Impact
