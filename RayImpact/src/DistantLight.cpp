#include "DistantLight.hpp"
#include "math.hpp"
#include "Scene.hpp"

namespace Impact {
namespace RayImpact {

// DistantLight method implementations

DistantLight::DistantLight(const Transformation& light_to_world,
                           const Vector3F& direction,
                           const RadianceSpectrum& incident_radiance)
    : Light::Light(LightFlags(LIGHT_DIRECTION_IS_DELTA),
                   light_to_world,
                   MediumInterface()),
    direction(light_to_world(direction)),
    incident_radiance(incident_radiance)
{}

void DistantLight::preprocess(const Scene& scene)
{
    scene.worldSpaceBoundingBox().boundingSphere(&scene_center, &scene_radius);
}

RadianceSpectrum DistantLight::sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                                      const Point2F& uniform_sample,
                                                      Vector3F* incident_direction,
                                                      imp_float* pdf_value,
                                                      VisibilityTester* visibility_tester) const
{
    *incident_direction = direction;

    *pdf_value = 1.0f;

    const Point3F& outside_point = scattering_event.position + direction*(2*scene_radius);

    *visibility_tester = VisibilityTester(ScatteringEvent(outside_point, medium_interface, scattering_event.time), scattering_event);

    return incident_radiance;
}

PowerSpectrum DistantLight::emittedPower() const
{
    return (IMP_PI*scene_radius*scene_radius)*incident_radiance;
}

// DistantLight creation

std::shared_ptr<Light> createDistantLight(const Transformation& light_to_world,
                                          const MediumInterface& medium_interface,
                                          const ParameterSet& parameters)
{
    const Vector3F& direction = parameters.getSingleVector3FValue("direction", Vector3F(0, 0, -1));
    const RadianceSpectrum& incident_radiance = parameters.getSingleSpectrumValue("incident_radiance", RadianceSpectrum(0.0f));

    return std::make_shared<DistantLight>(light_to_world,
                                          direction,
                                          incident_radiance);
}

} // RayImpact
} // Impact
