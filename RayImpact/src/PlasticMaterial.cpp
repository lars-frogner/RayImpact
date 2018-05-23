#include "PlasticMaterial.hpp"
#include "memory.hpp"
#include "MicrofacetDistribution.hpp"
#include "LambertianBRDF.hpp"
#include "MicrofacetBRDF.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// PlasticMaterial method definitions

void PlasticMaterial::generateBSDF(SurfaceScatteringEvent* scattering_event,
                                   RegionAllocator& allocator,
                                   TransportMode transport_mode,
                                   bool allow_multiple_scattering_types) const
{
    if (bump_map)
        performBumpMapping(bump_map, scattering_event);

    scattering_event->bsdf = allocated_in_region(allocator, BSDF)(*scattering_event);

    const ReflectionSpectrum& diffuse_reflectance = diffuse_reflectance_texture->evaluate(*scattering_event).clamped();

    if (!diffuse_reflectance.isBlack())
        scattering_event->bsdf->addComponent(allocated_in_region(allocator, LambertianBRDF)(diffuse_reflectance));

    const ReflectionSpectrum& glossy_reflectance = glossy_reflectance_texture->evaluate(*scattering_event).clamped();

    if (!glossy_reflectance.isBlack())
    {
        FresnelReflector* fresnel_reflector = allocated_in_region(allocator, DielectricReflector)(1.0f, 1.5f);

        imp_float slope_deviation = roughness_texture->evaluate(*scattering_event);

        if (normalized_roughness)
            slope_deviation = TrowbridgeReitzDistribution::roughnessToDeviation(slope_deviation);

        MicrofacetDistribution* microfacet_distribution = allocated_in_region(allocator, TrowbridgeReitzDistribution)(slope_deviation,
                                                                                                                      slope_deviation,
																													  false);

        scattering_event->bsdf->addComponent(allocated_in_region(allocator, MicrofacetBRDF)(glossy_reflectance,
                                                                                            microfacet_distribution,
                                                                                            fresnel_reflector));
    }
}

// PlasticMaterial function definitions

Material* createPlasticMaterial(const TextureParameterSet& parameters)
{
	const std::shared_ptr< Texture<ReflectionSpectrum> >& diffuse_reflectance = parameters.getSpectrumTexture("diffuse_reflectance", ReflectionSpectrum(0.25f));
	const std::shared_ptr< Texture<ReflectionSpectrum> >& glossy_reflectance = parameters.getSpectrumTexture("glossy_reflectance", ReflectionSpectrum(0.25f));
	const std::shared_ptr< Texture<imp_float> >& roughness = parameters.getFloatTexture("roughness", 0.1f);
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
						 "\n    %-20s%s",
						 "Type:", "Plastic",
						 "Diffuse reflectance:", diffuse_reflectance->toString().c_str(),
						 "Glossy reflectance:", glossy_reflectance->toString().c_str(),
						 "Roughness:", roughness->toString().c_str(),
						 "Bump map:", (bump_map)? bump_map->toString().c_str() : "none",
						 "Norm. roughness:", normalized_roughness? "yes" : "no");
	}

    return new PlasticMaterial(diffuse_reflectance,
                               glossy_reflectance,
                               roughness,
                               bump_map,
                               normalized_roughness);
}

} // RayImpact
} // Impact
