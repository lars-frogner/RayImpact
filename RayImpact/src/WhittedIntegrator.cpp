#include "WhittedIntegrator.hpp"
#include "BSDF.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// WhittedIntegrator method implementations

WhittedIntegrator::WhittedIntegrator(std::shared_ptr<const Camera> camera,
                                     std::shared_ptr<Sampler> sampler,
                                     unsigned int max_scattering_count)
    : SampleIntegrator::SampleIntegrator(camera, sampler),
      max_scattering_count(max_scattering_count)
{}

RadianceSpectrum WhittedIntegrator::incidentRadiance(const RayWithOffsets& outgoing_ray,
                                                     const Scene& scene,
                                                     Sampler& sampler,
                                                     RegionAllocator& allocator,
                                                     unsigned int scattering_count /* = 0 */) const
{
    RadianceSpectrum total_incident_radiance(0.0f);

    SurfaceScatteringEvent scattering_event;

    if (!scene.intersect(outgoing_ray, &scattering_event))
    {
        for (const auto& light : scene.lights)
            total_incident_radiance += light->emittedRadianceFromDirection(outgoing_ray);

        return total_incident_radiance;
    }

    Vector3F outgoing_direction = scattering_event.outgoing_direction;

    scattering_event.generateBSDF(outgoing_ray, allocator);

    total_incident_radiance += scattering_event.emittedRadiance(outgoing_direction);

    for (const auto& light : scene.lights)
    {
        Vector3F incident_direction;
        imp_float pdf_value;
        VisibilityTester visibility_tester;

        const RadianceSpectrum& incident_radiance = light->sampleIncidentRadiance(scattering_event,
                                                                                  sampler.next2DSampleComponent(),
                                                                                  &incident_direction,
                                                                                  &pdf_value,
                                                                                  &visibility_tester);

        if (incident_radiance.isBlack() || pdf_value == 0)
            continue;

        const Spectrum& bsdf_value = scattering_event.bsdf->evaluate(outgoing_direction, incident_direction);

        if (!bsdf_value.isBlack() && visibility_tester.beamIsUnobstructed(scene))
            total_incident_radiance += bsdf_value*incident_radiance*(incident_direction.absDot(scattering_event.surface_normal)/pdf_value);
    }

    if (scattering_count + 1 < max_scattering_count)
    {
        total_incident_radiance += specularlyReflectedRadiance(outgoing_ray, scattering_event, scene, sampler, allocator, scattering_count);
        total_incident_radiance += specularlyTransmittedRadiance(outgoing_ray, scattering_event, scene, sampler, allocator, scattering_count);
    }

    return total_incident_radiance;
}

// WhittedIntegrator creation

Integrator* createWhittedIntegrator(std::shared_ptr<const Camera> camera,
                                    std::shared_ptr<Sampler> sampler,
                                    const ParameterSet& parameters)
{
    unsigned int max_scattering_count = (unsigned int)std::abs(parameters.getSingleIntValue("max_scattering_count", 5));

    return new WhittedIntegrator(camera, sampler, max_scattering_count);
}

} // RayImpact
} // Impact
