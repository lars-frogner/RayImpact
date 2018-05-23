#include "Light.hpp"
#include "Scene.hpp"

namespace Impact {
namespace RayImpact {

// VisibilityTester method definitions

bool VisibilityTester::beamIsUnobstructed(const Scene& scene) const
{
    return !scene.hasIntersection(start_point.spawnRayTo(end_point));
}

TransmissionSpectrum VisibilityTester::beamTransmittance(const Scene& scene, Sampler& sampler) const
{
    Ray ray(start_point.spawnRayTo(end_point));

    TransmissionSpectrum transmittance(1.0f);

    while (true)
    {
        SurfaceScatteringEvent scattering_event;
        bool hit_surface = scene.intersect(ray, &scattering_event);

        if (hit_surface && scattering_event.model->getMaterial() != nullptr)
            return TransmissionSpectrum(0.0f);

        if (ray.medium)
            transmittance *= TransmissionSpectrum(1.0f);//ray.medium->transmittance(ray, sampler);

        if (!hit_surface)
            break;

        ray = scattering_event.spawnRayTo(end_point);
    }

    return transmittance;
}

} // RayImpact
} // Impact
