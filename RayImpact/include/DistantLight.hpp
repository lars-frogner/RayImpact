#pragma once
#include "Light.hpp"
#include "ParameterSet.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// DistantLight declarations

class DistantLight : public Light {

private:

    const Vector3F direction; // The world-space direction of the light
    const RadianceSpectrum incident_radiance; // The radiance recieved from the light
    Point3F scene_center; // Center of scene bounding sphere
    imp_float scene_radius; // Radius of scene bounding sphere

public:

    DistantLight(const Transformation& light_to_world,
                 const Vector3F& direction,
                 const RadianceSpectrum& radiance);

    void DistantLight::preprocess(const Scene& scene);

    RadianceSpectrum sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                            const Point2F& uniform_sample,
                                            Vector3F* incident_direction,
                                            imp_float* pdf_value,
                                            VisibilityTester* visibility_tester) const;

    PowerSpectrum emittedPower() const;
};

// DistantLight creation

std::shared_ptr<Light> createDistantLight(const Transformation& light_to_world,
                                          const MediumInterface& medium_interface,
                                          const ParameterSet& parameters);

} // RayImpact
} // Impact
