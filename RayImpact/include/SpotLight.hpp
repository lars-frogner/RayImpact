#pragma once
#include "Light.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// SpotLight declarations

class SpotLight : public Light {

private:

    const Point3F position; // The world-space position of the light
    const IntensitySpectrum emitted_intensity; // The power per solid angle emitted by the light
    const imp_float cos_max_angle; // Cosine of the angular width of the full light cone
    const imp_float cos_falloff_start_angle; // Cosine of the angular width of the interior (full-intensity) light cone

    imp_float falloffInDirection(const Vector3F& direction) const;

public:

    SpotLight(const Transformation& light_to_world,
              const MediumInterface& medium_interface,
              const IntensitySpectrum& emitted_intensity,
              imp_float max_angle,
              imp_float falloff_start_angle);

    RadianceSpectrum sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                            const Point2F& uniform_sample,
                                            Vector3F* incident_direction,
                                            imp_float* pdf_value,
                                            VisibilityTester* visibility_tester) const;

    PowerSpectrum emittedPower() const;
};

// SpotLight creation

std::shared_ptr<Light> createSpotLight(const Transformation& light_to_world,
                                       const MediumInterface& medium_interface,
                                       const ParameterSet& parameters);

} // RayImpact
} // Impact
