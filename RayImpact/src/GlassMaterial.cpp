#include "GlassMaterial.hpp"
#include "memory.hpp"
#include "math.hpp"
#include "BSDF.hpp"
#include "MicrofacetDistribution.hpp"
#include "SpecularBSDF.hpp"
#include "FresnelReflector.hpp"
#include "SpecularBRDF.hpp"
#include "SpecularBTDF.hpp"
#include "MicrofacetBRDF.hpp"
#include "MicrofacetBTDF.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// GlassMaterial method definitions

void GlassMaterial::generateBSDF(SurfaceScatteringEvent* scattering_event,
                                 RegionAllocator& allocator,
                                 TransportMode transport_mode,
                                 bool allow_multiple_scattering_types) const
{
    if (bump_map)
        performBumpMapping(bump_map, scattering_event);

	imp_float refractive_index = refractive_index_texture->evaluate(*scattering_event);
	
    const ReflectionSpectrum& reflectance = reflectance_texture->evaluate(*scattering_event).clamped();
    const ReflectionSpectrum& transmittance = transmittance_texture->evaluate(*scattering_event).clamped();
	
    scattering_event->bsdf = allocated_in_region(allocator, BSDF)(*scattering_event, refractive_index);

	if (reflectance.isBlack() && transmittance.isBlack())
		return;

    imp_float slope_deviation_x = roughness_u_texture->evaluate(*scattering_event);
    imp_float slope_deviation_y = roughness_v_texture->evaluate(*scattering_event);

	bool is_specular = slope_deviation_x == 0 && slope_deviation_y == 0;

	if (is_specular && allow_multiple_scattering_types)
	{
		scattering_event->bsdf->addComponent(allocated_in_region(allocator, SpecularBSDF)(reflectance,
																						  transmittance,
																						  1.0f, refractive_index,
																						  transport_mode));
	}
	else
	{
		if (normalized_roughness)
		{
			slope_deviation_x = TrowbridgeReitzDistribution::roughnessToDeviation(slope_deviation_x);
			slope_deviation_y = TrowbridgeReitzDistribution::roughnessToDeviation(slope_deviation_y);
		}

		FresnelReflector* fresnel_reflector = nullptr;
		if (!reflectance.isBlack())
			fresnel_reflector = allocated_in_region(allocator, DielectricReflector)(1.0f, refractive_index);

		if (is_specular)
		{
			if (!reflectance.isBlack())
				scattering_event->bsdf->addComponent(allocated_in_region(allocator, SpecularBRDF)(reflectance, fresnel_reflector));

			if (!transmittance.isBlack())
				scattering_event->bsdf->addComponent(allocated_in_region(allocator, SpecularBTDF)(transmittance, 
																								  1.0f, refractive_index,
																								  transport_mode));
		}
		else
		{
			MicrofacetDistribution* microfacet_distribution = allocated_in_region(allocator, TrowbridgeReitzDistribution)(slope_deviation_x,
																														  slope_deviation_y,
																														  false);
		
			if (!reflectance.isBlack())
				scattering_event->bsdf->addComponent(allocated_in_region(allocator, MicrofacetBRDF)(reflectance,
																									microfacet_distribution,
																									fresnel_reflector));

			if (!transmittance.isBlack())
				scattering_event->bsdf->addComponent(allocated_in_region(allocator, MicrofacetBTDF)(transmittance,
																									1.0f, refractive_index,
																									microfacet_distribution,
																									transport_mode));
		}

	}
}

// GlassMaterial function definitions

Material* createGlassMaterial(const TextureParameterSet& parameters)
{
	const std::shared_ptr< Texture<ReflectionSpectrum> >& reflectance = parameters.getSpectrumTexture("reflectance", ReflectionSpectrum(0.5f));
	const std::shared_ptr< Texture<TransmissionSpectrum> >& transmittance = parameters.getSpectrumTexture("transmittance", TransmissionSpectrum(0.5f));
	const std::shared_ptr< Texture<imp_float> >& roughness_u_dir = parameters.getFloatTexture("roughness_u_dir", 0.0f);
	const std::shared_ptr< Texture<imp_float> >& roughness_v_dir = parameters.getFloatTexture("roughness_v_dir", 0.0f);
	const std::shared_ptr< Texture<imp_float> >& refractive_index = parameters.getFloatTexture("refractive_index", 1.5f);
	const std::shared_ptr< Texture<imp_float> >& bump_map = parameters.getFloatTexture("bump_map");
    bool normalized_roughness = parameters.getSingleBoolValue("normalized_roughness", true);
    
	if (RIMP_OPTIONS.verbosity >= IMP_MATERIALS_VERBOSITY)
	{
		printInfoMessage("Material:"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Type:", "Glass",
						 "Reflectance:", reflectance->toString().c_str(),
						 "Transmittance:", transmittance->toString().c_str(),
						 "Roughness (u-dir.):", roughness_u_dir->toString().c_str(),
						 "Roughness (v-dir.):", roughness_v_dir->toString().c_str(),
						 "Refractive index:", refractive_index->toString().c_str(),
						 "Bump map:", (bump_map)? bump_map->toString().c_str() : "none",
						 "Norm. roughness:", normalized_roughness? "yes" : "no");
	}

    return new GlassMaterial(reflectance,
							 transmittance,
                             roughness_u_dir,
                             roughness_v_dir,
							 refractive_index,
                             bump_map,
							 normalized_roughness);
}

} // RayImpact
} // Impact
