#include "MatteMaterial.hpp"
#include "memory.hpp"
#include "math.hpp"
#include "BSDF.hpp"
#include "LambertianBRDF.hpp"
#include "OrenNayarBRDF.hpp"

namespace Impact {
namespace RayImpact {

// MatteMaterial method implementations

MatteMaterial::MatteMaterial(const std::shared_ptr< Texture<ReflectionSpectrum> >& reflectance_texture,
                             const std::shared_ptr< Texture<imp_float> >& slope_deviation_texture,
                             const std::shared_ptr< Texture<imp_float> >& bump_map)
    : reflectance_texture(reflectance_texture),
      slope_deviation_texture(slope_deviation_texture),
      bump_map(bump_map)
{}

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

} // RayImpact
} // Impact
