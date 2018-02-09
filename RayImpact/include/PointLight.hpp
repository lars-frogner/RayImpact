#pragma once
#include "Light.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// PointLight declarations

class PointLight : public Light {

private:

    const Point3F position; // The world-space position of the light
    const IntensitySpectrum emitted_intensity; // The power per solid angle emitted by the light

public:

    PointLight(const Transformation& light_to_world,
               const MediumInterface& medium_interface,
               const IntensitySpectrum& emitted_intensity);

    RadianceSpectrum sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                            const Point2F& uniform_sample,
                                            Vector3F* incident_direction,
                                            imp_float* pdf_value,
                                            VisibilityTester* visibility_tester) const;

    PowerSpectrum emittedPower() const;
};

// PointLight creation

std::shared_ptr<Light> createPointLight(const Transformation& light_to_world,
                                        const MediumInterface& medium_interface,
                                        const ParameterSet& parameters);

} // RayImpact
} // Impact
