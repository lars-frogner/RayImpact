#include "Material.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// Material method implementations
    
void Material::performBumpMapping(const std::shared_ptr< Texture<imp_float> >& bump_map,
                                  SurfaceScatteringEvent* scattering_event) const
{
    const Normal3F& original_shading_normal = Normal3F(scattering_event->shading.dpdu.cross(scattering_event->shading.dpdv));

    SurfaceScatteringEvent shifted_scattering_event(*scattering_event);

    // Set shift amount in the u-direction
    imp_float du = 0.5f*(std::abs(scattering_event->dudx) + std::abs(scattering_event->dudy));
    if (du == 0)
        du = 0.01f;

    // Create scattering event shifted slightly in the u-direction
    shifted_scattering_event.position = scattering_event->position + du*scattering_event->shading.dpdu;
    shifted_scattering_event.position_uv = scattering_event->position_uv + Vector2F(du, 0);
    shifted_scattering_event.surface_normal = (original_shading_normal + du*scattering_event->dndu).normalized();

    // Evaluate displacement amount at the u-shifted location
    imp_float u_shifted_displacement = bump_map->evaluate(shifted_scattering_event);

    // Set shift amount in the v-direction
    imp_float dv = 0.5f*(std::abs(scattering_event->dvdx) + std::abs(scattering_event->dvdy));
    if (dv == 0)
        dv = 0.01f;

    // Create scattering event shifted slightly in the v-direction
    shifted_scattering_event.position = scattering_event->position + dv*scattering_event->shading.dpdv;
    shifted_scattering_event.position_uv = scattering_event->position_uv + Vector2F(0, dv);
    shifted_scattering_event.surface_normal = (original_shading_normal + dv*scattering_event->dndv).normalized();

    // Evaluate displacement amount at the v-shifted location
    imp_float v_shifted_displacement = bump_map->evaluate(shifted_scattering_event);

    // Evaluate displacement amount at the unshifted location
    imp_float displacement = bump_map->evaluate(*scattering_event);

    // Compute position derivatives of the bumped surface

    const Vector3F& bumped_dpdu = scattering_event->shading.dpdu +
                                  Vector3F(scattering_event->shading.surface_normal)*((u_shifted_displacement - displacement)/du) +
                                  Vector3F(scattering_event->shading.dndu)*displacement;

    const Vector3F& bumped_dpdv = scattering_event->shading.dpdv +
                                  Vector3F(scattering_event->shading.surface_normal)*((v_shifted_displacement - displacement)/dv) +
                                  Vector3F(scattering_event->shading.dndv)*displacement;

    // Update shading geometry of the scattering event
    scattering_event->setShadingGeometry(bumped_dpdu, bumped_dpdv,
                                         scattering_event->shading.dndu, scattering_event->shading.dndv,
                                         false);
}

} // RayImpact
} // Impact
