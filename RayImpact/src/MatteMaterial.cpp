#include "MatteMaterial.hpp"
#include "memory.hpp"
#include "math.hpp"
#include "BSDF.hpp"
#include "LambertianBRDF.hpp"
#include "OrenNayarBRDF.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// MatteMaterial method definitions

void MatteMaterial::generateBSDF(SurfaceScatteringEvent* scattering_event,
                                 RegionAllocator& allocator,
                                 TransportMode transport_mode,
                                 bool allow_multiple_scattering_types) const
{
    if (bump_map)
        performBumpMapping(bump_map, scattering_event);

    scattering_event->bsdf = allocated_in_region(allocator, BSDF)(*scattering_event);

    const ReflectionSpectrum& reflectance = reflectance_texture->evaluate(*scattering_event).clamped();

    imp_float slope_deviation = clamp(slope_deviation_texture->evaluate(*scattering_event), 0.0f, 90.0f);

    if (!reflectance.isBlack())
    {
        if (slope_deviation == 0)
            scattering_event->bsdf->addComponent(allocated_in_region(allocator, LambertianBRDF)(reflectance));
        else
            scattering_event->bsdf->addComponent(allocated_in_region(allocator, OrenNayarBRDF)(reflectance, slope_deviation));
    }
}

// MatteMaterial function definitions

Material* createMatteMaterial(const TextureParameterSet& parameters)
{
	const std::shared_ptr< Texture<ReflectionSpectrum> >& reflectance = parameters.getSpectrumTexture("reflectance", ReflectionSpectrum(0.5f));
	const std::shared_ptr< Texture<imp_float> >& roughness = parameters.getFloatTexture("roughness", 0.0f);
	const std::shared_ptr< Texture<imp_float> >& bump_map = parameters.getFloatTexture("bump_map");
    
	if (RIMP_OPTIONS.verbosity >= IMP_MATERIALS_VERBOSITY)
	{
		printInfoMessage("Material:"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s"
						 "\n    %-20s%s",
						 "Type:", "Matte",
						 "Reflectance:", reflectance->toString().c_str(),
						 "Roughness:", roughness->toString().c_str(),
						 "Bump map:", (bump_map)? bump_map->toString().c_str() : "none");
	}

    return new MatteMaterial(reflectance,
                             roughness,
                             bump_map);
}

} // RayImpact
} // Impact
