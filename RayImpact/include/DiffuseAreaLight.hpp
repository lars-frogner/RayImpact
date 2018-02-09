#pragma once
#include "Light.hpp"
#include "Shape.hpp"
#include "ParameterSet.hpp"
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

// DiffuseAreaLight creation

std::shared_ptr<AreaLight> createDiffuseAreaLight(const Transformation& light_to_world,
                                                  const MediumInterface& medium_interface,
                                                  const ParameterSet& parameters,
                                                  std::shared_ptr<Shape> shape);

} // RayImpact
} // Impact
