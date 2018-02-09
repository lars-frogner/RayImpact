#include "Light.hpp"
#include "error.hpp"
#include "Scene.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// Light method implementations

Light::Light(LightFlags flags,
             const Transformation& light_to_world,
             const MediumInterface& medium_interface,
             unsigned int n_samples /* = 1 */)
    : flags(flags),
      light_to_world(light_to_world),
      world_to_light(light_to_world.inverted()),
      medium_interface(medium_interface),
      n_samples(std::max(1u, n_samples))
{
    if (light_to_world.hasScaling())
        printWarningMessage("light-to-world transformation with scaling detected");
}

void Light::preprocess(const Scene& scene)
{}
   
RadianceSpectrum Light::emittedRadianceFromDirection(const RayWithOffsets& ray) const
{
    return RadianceSpectrum(0.0f);
}

// AreaLight method implementations

AreaLight::AreaLight(LightFlags flags,
                     const Transformation& light_to_world,
                     const MediumInterface& medium_interface,
                     unsigned int n_samples /* = 1 */)
    : Light::Light(LightFlags(flags | LIGHT_HAS_AREA), light_to_world, medium_interface, n_samples)
{}

AreaLight::AreaLight(const Transformation& light_to_world,
                     const MediumInterface& medium_interface,
                     unsigned int n_samples /* = 1 */)
    : Light::Light(LightFlags(LIGHT_HAS_AREA), light_to_world, medium_interface, n_samples)
{}

// VisibilityTester method implementations

VisibilityTester::VisibilityTester()
    : start_point(), end_point()
{}

VisibilityTester::VisibilityTester(const ScatteringEvent& start_point,
                                   const ScatteringEvent& end_point)
    : start_point(start_point), end_point(end_point)
{}

const ScatteringEvent& VisibilityTester::startPoint() const
{
    return start_point;
}

const ScatteringEvent& VisibilityTester::endPoint() const
{
    return end_point;
}

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
            transmittance *= ray.medium->transmittance(ray, sampler);

        if (!hit_surface)
            break;

        ray = scattering_event.spawnRayTo(end_point);
    }

    return transmittance;
}

} // RayImpact
} // Impact
