#include "PlasticMaterial.hpp"
#include "memory.hpp"
#include "MicrofacetDistribution.hpp"
#include "LambertianBRDF.hpp"
#include "MicrofacetBRDF.hpp"

namespace Impact {
namespace RayImpact {

// PlasticMaterial method implementations

PlasticMaterial::PlasticMaterial(const std::shared_ptr< Texture<ReflectionSpectrum> >& diffuse_reflectance_texture,
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
                                                                                                                      slope_deviation);

        scattering_event->bsdf->addComponent(allocated_in_region(allocator, MicrofacetBRDF)(glossy_reflectance,
                                                                                            microfacet_distribution,
                                                                                            fresnel_reflector));
    }
}

// PlasticMaterial creation

Material* createPlasticMaterial(const TextureParameterSet& parameters)
{
    bool normalized_roughness = parameters.getSingleBoolValue("normalized_roughness", true);

    return new PlasticMaterial(parameters.getSpectrumTexture("diffuse_reflectance", ReflectionSpectrum(0.0f)),
                               parameters.getSpectrumTexture("glossy_reflectance", ReflectionSpectrum(0.0f)),
                               parameters.getFloatTexture("roughness", 0.0f),
                               parameters.getFloatTexture("bump_map"),
                               normalized_roughness);
}

} // RayImpact
} // Impact
