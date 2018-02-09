#include "DiffuseAreaLight.hpp"
#include "math.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// DiffuseAreaLight method implementations

DiffuseAreaLight::DiffuseAreaLight(const Transformation& light_to_world,
                                   const MediumInterface& medium_interface,
                                   const RadianceSpectrum& emitted_radiance,
                                   unsigned int n_samples,
                                   const std::shared_ptr<Shape>& shape)
    : AreaLight::AreaLight(light_to_world, medium_interface, n_samples),
      emitted_radiance(emitted_radiance),
      shape(shape),
      surface_area(shape->surfaceArea())
{}

RadianceSpectrum DiffuseAreaLight::sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                                          const Point2F& uniform_sample,
                                                          Vector3F* incident_direction,
                                                          imp_float* pdf_value,
                                                          VisibilityTester* visibility_tester) const
{
    printSevereMessage("\"DiffuseAreaLight::sampleIncidentRadiance\" has not been implemented");
    return RadianceSpectrum(0.0f);
}

RadianceSpectrum DiffuseAreaLight::emittedRadiance(const ScatteringEvent& scattering_event,
                                                   const Vector3F& outgoing_direction) const
{
    return (outgoing_direction.dot(scattering_event.surface_normal) > 0)? emitted_radiance : RadianceSpectrum(0.0f);
}

PowerSpectrum DiffuseAreaLight::emittedPower() const
{
    return (IMP_PI*surface_area)*emitted_radiance;
}

// DiffuseAreaLight creation

std::shared_ptr<AreaLight> createDiffuseAreaLight(const Transformation& light_to_world,
                                                  const MediumInterface& medium_interface,
                                                  const ParameterSet& parameters,
                                                  std::shared_ptr<Shape> shape)
{
    const RadianceSpectrum& emitted_radiance = parameters.getSingleSpectrumValue("emitted_radiance", RadianceSpectrum(0.0f));
    unsigned int n_samples = (unsigned int)std::abs(parameters.getSingleIntValue("n_samples", 1));

    return std::make_shared<DiffuseAreaLight>(light_to_world,
                                              medium_interface,
                                              emitted_radiance,
                                              n_samples,
                                              shape);
}

} // RayImpact
} // Impact
