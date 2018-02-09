#include "PointLight.hpp"
#include "math.hpp"

namespace Impact {
namespace RayImpact {

// PointLight method implementations

PointLight::PointLight(const Transformation& light_to_world,
                       const MediumInterface& medium_interface,
                       const IntensitySpectrum& emitted_intensity)
    : Light::Light(LightFlags(LIGHT_POSITION_IS_DELTA),
                   light_to_world,
                   medium_interface),
    position(light_to_world(Point3F(0, 0, 0))),
    emitted_intensity(emitted_intensity)
{}

RadianceSpectrum PointLight::sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                                    const Point2F& uniform_sample,
                                                    Vector3F* incident_direction,
                                                    imp_float* pdf_value,
                                                    VisibilityTester* visibility_tester) const
{
    *incident_direction = (position - scattering_event.position).normalized();

    *pdf_value = 1.0f;

    *visibility_tester = VisibilityTester(ScatteringEvent(position, medium_interface, scattering_event.time), scattering_event);

    return emitted_intensity/squaredDistanceBetween(position, scattering_event.position);
}

PowerSpectrum PointLight::emittedPower() const
{
    return IMP_FOUR_PI*emitted_intensity;
}

// PointLight creation

std::shared_ptr<Light> createPointLight(const Transformation& light_to_world,
                                        const MediumInterface& medium_interface,
                                        const ParameterSet& parameters)
{
    const IntensitySpectrum& emitted_intensity = parameters.getSingleSpectrumValue("emitted_intensity", RadianceSpectrum(0.0f));

    return std::make_shared<PointLight>(light_to_world,
                                        medium_interface,
                                        emitted_intensity);
}

} // RayImpact
} // Impact
