#pragma once
#include "Light.hpp"
#include "ParameterSet.hpp"
#include "math.hpp"
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

// SpotLight function declarations

std::shared_ptr<Light> createSpotLight(const Transformation& light_to_world,
                                       const MediumInterface& medium_interface,
                                       const ParameterSet& parameters);

// PointLight inline method definitions

inline SpotLight::SpotLight(const Transformation& light_to_world,
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

inline PowerSpectrum SpotLight::emittedPower() const
{
    return (IMP_TWO_PI*(1 - 0.5f*(cos_max_angle + cos_falloff_start_angle)))*emitted_intensity;
}

} // RayImpact
} // Impact
