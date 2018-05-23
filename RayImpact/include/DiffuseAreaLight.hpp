#pragma once
#include "Light.hpp"
#include "Shape.hpp"
#include "ParameterSet.hpp"
#include "math.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// DiffuseAreaLight declarations

class DiffuseAreaLight : public AreaLight {

private:

    const RadianceSpectrum emitted_radiance; // The radiance emitted from each point on the surface
    std::shared_ptr<Shape> shape; // The shape representing the emitting surface
    const imp_float surface_area; // The total area of the surface

public:

    DiffuseAreaLight(const Transformation& light_to_world,
                     const MediumInterface& medium_interface,
                     const RadianceSpectrum& emitted_radiance,
                     unsigned int n_samples,
                     const std::shared_ptr<Shape>& shape);

    RadianceSpectrum sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                            const Point2F& uniform_sample,
                                            Vector3F* incident_direction,
                                            imp_float* pdf_value,
                                            VisibilityTester* visibility_tester) const;

    RadianceSpectrum emittedRadiance(const ScatteringEvent& scattering_event,
                                     const Vector3F& outgoing_direction) const;

    PowerSpectrum emittedPower() const;
};

// DiffuseAreaLight function declarations

std::shared_ptr<AreaLight> createDiffuseAreaLight(const Transformation& light_to_world,
                                                  const MediumInterface& medium_interface,
                                                  const ParameterSet& parameters,
                                                  std::shared_ptr<Shape> shape);

// DiffuseAreaLight inline method definitions

inline DiffuseAreaLight::DiffuseAreaLight(const Transformation& light_to_world,
										  const MediumInterface& medium_interface,
										  const RadianceSpectrum& emitted_radiance,
										  unsigned int n_samples,
										  const std::shared_ptr<Shape>& shape)
    : AreaLight::AreaLight(light_to_world, medium_interface, n_samples),
      emitted_radiance(emitted_radiance),
      shape(shape),
      surface_area(shape->surfaceArea())
{}

inline RadianceSpectrum DiffuseAreaLight::sampleIncidentRadiance(const ScatteringEvent& scattering_event,
																 const Point2F& uniform_sample,
																 Vector3F* incident_direction,
																 imp_float* pdf_value,
																 VisibilityTester* visibility_tester) const
{
    printSevereMessage("\"DiffuseAreaLight::sampleIncidentRadiance\" has not been implemented");
    return RadianceSpectrum(0.0f);
}

inline RadianceSpectrum DiffuseAreaLight::emittedRadiance(const ScatteringEvent& scattering_event,
														  const Vector3F& outgoing_direction) const
{
    return (outgoing_direction.dot(scattering_event.surface_normal) > 0)? emitted_radiance : RadianceSpectrum(0.0f);
}

inline PowerSpectrum DiffuseAreaLight::emittedPower() const
{
    return (IMP_PI*surface_area)*emitted_radiance;
}

} // RayImpact
} // Impact
