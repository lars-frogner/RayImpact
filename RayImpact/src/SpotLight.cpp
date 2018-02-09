#include "SpotLight.hpp"
#include "math.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// PointLight method implementations

SpotLight::SpotLight(const Transformation& light_to_world,
                     const MediumInterface& medium_interface,
                     const IntensitySpectrum& emitted_intensity,
                     imp_float max_angle,
                     imp_float falloff_start_angle)
    : Light::Light(LightFlags(LIGHT_POSITION_IS_DELTA),
                   light_to_world,
                   medium_interface),
    position(light_to_world(Point3F(0, 0, 0))),
    emitted_intensity(emitted_intensity),
    cos_max_angle(std::cos(degreesToRadians(clamp(max_angle, 0.0f, 180.0f)))),
    cos_falloff_start_angle(std::cos(degreesToRadians(clamp(falloff_start_angle, 0.0f, max_angle))))
{}

imp_float SpotLight::falloffInDirection(const Vector3F& direction) const
{
    const Vector3F& direction_in_light_system = world_to_light(direction).normalized();

    imp_float cos_direction_angle = direction_in_light_system.z;

    if (cos_direction_angle < cos_max_angle)
        return 0;

    if (cos_direction_angle > cos_falloff_start_angle)
        return 1;

    imp_float delta = (cos_direction_angle - cos_max_angle)/(cos_falloff_start_angle - cos_max_angle);

    return (delta*delta)*(delta*delta);
}

RadianceSpectrum SpotLight::sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                                    const Point2F& uniform_sample,
                                                    Vector3F* incident_direction,
                                                    imp_float* pdf_value,
                                                    VisibilityTester* visibility_tester) const
{
    *incident_direction = (position - scattering_event.position).normalized();

    *pdf_value = 1.0f;

    *visibility_tester = VisibilityTester(ScatteringEvent(position, medium_interface, scattering_event.time), scattering_event);

    return emitted_intensity*(falloffInDirection(-(*incident_direction))/squaredDistanceBetween(position, scattering_event.position));
}

PowerSpectrum SpotLight::emittedPower() const
{
    return (IMP_TWO_PI*(1 - 0.5f*(cos_max_angle + cos_falloff_start_angle)))*emitted_intensity;
}

// SpotLight creation

std::shared_ptr<Light> createSpotLight(const Transformation& light_to_world,
                                       const MediumInterface& medium_interface,
                                       const ParameterSet& parameters)
{
    const IntensitySpectrum& emitted_intensity = parameters.getSingleSpectrumValue("emitted_intensity", RadianceSpectrum(0.0f));
    imp_float max_angle = parameters.getSingleFloatValue("max_angle", 180.0f);
    imp_float falloff_start_angle = parameters.getSingleFloatValue("falloff_start_angle", max_angle);

    return std::make_shared<SpotLight>(light_to_world,
                                       medium_interface,
                                       emitted_intensity,
                                       max_angle,
                                       falloff_start_angle);
}

} // RayImpact
} // Impact
