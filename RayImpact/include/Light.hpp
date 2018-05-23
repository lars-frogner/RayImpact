#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include "Transformation.hpp"
#include "Ray.hpp"
#include "Spectrum.hpp"
#include "ScatteringEvent.hpp"
#include "Sampler.hpp"
#include "error.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// Forward declarations
class Scene;

// VisibilityTester declarations

class VisibilityTester {

private:

    ScatteringEvent start_point; // Scattering event representing the start point of a light ray
    ScatteringEvent end_point; // Scattering event representing the end point of a light ray

public:

    VisibilityTester();

    VisibilityTester(const ScatteringEvent& start_point,
                     const ScatteringEvent& end_point);

    const ScatteringEvent& startPoint() const;
    const ScatteringEvent& endPoint() const;

    bool beamIsUnobstructed(const Scene& scene) const;

    TransmissionSpectrum beamTransmittance(const Scene& scene, Sampler& sampler) const;
};

// Light declarations

enum LightFlags
{
    LIGHT_POSITION_IS_DELTA  = 1 << 0,
    LIGHT_DIRECTION_IS_DELTA = 1 << 1,
    LIGHT_HAS_AREA           = 1 << 2,
    LIGHT_IS_INFINITE        = 1 << 3
};

class Light {

protected:

    const Transformation light_to_world; // Transformation from the light system to the world system
    const Transformation world_to_light; // Transformation from the world system to the light system

public:

    const LightFlags flags; // Flags specifying properties of the light

    const MediumInterface medium_interface; // Interface between the media on the inside and outside of the light

    const unsigned int n_samples; // Number of samples to take of the light surface

    Light(LightFlags flags,
          const Transformation& light_to_world,
          const MediumInterface& medium_interface,
          unsigned int n_samples = 1);

    virtual void preprocess(const Scene& scene);

    virtual RadianceSpectrum sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                                    const Point2F& uniform_sample,
                                                    Vector3F* incident_direction,
                                                    imp_float* pdf_value,
                                                    VisibilityTester* visibility_tester) const = 0;

    virtual RadianceSpectrum emittedRadianceFromDirection(const RayWithOffsets& ray) const;

    virtual PowerSpectrum emittedPower() const = 0;
};

// AreaLight declarations

class AreaLight : public Light {

public:

    AreaLight(LightFlags flags,
              const Transformation& light_to_world,
              const MediumInterface& medium_interface,
              unsigned int n_samples = 1);

    AreaLight(const Transformation& light_to_world,
              const MediumInterface& medium_interface,
              unsigned int n_samples = 1);

    virtual RadianceSpectrum emittedRadiance(const ScatteringEvent& scattering_event,
                                             const Vector3F& outgoing_direction) const = 0;
};

// Light inline function definitions

inline bool lightIsDelta(LightFlags flags)
{
    return (flags & LIGHT_POSITION_IS_DELTA) || (flags & LIGHT_DIRECTION_IS_DELTA);
}

// Light inline method definitions

inline Light::Light(LightFlags flags,
					const Transformation& light_to_world,
					const MediumInterface& medium_interface,
					unsigned int n_samples /* = 1 */)
    : flags(flags),
      light_to_world(light_to_world),
      world_to_light(light_to_world.inverted()),
      medium_interface(medium_interface),
      n_samples(std::max(1u, n_samples))
{
    if (light_to_world.hasScaling())
        printWarningMessage("light-to-world transformation with scaling detected");
}

inline void Light::preprocess(const Scene& scene)
{}

inline RadianceSpectrum Light::emittedRadianceFromDirection(const RayWithOffsets& ray) const
{
    return RadianceSpectrum(0.0f);
}

// AreaLight inline method definitions

inline AreaLight::AreaLight(LightFlags flags,
							const Transformation& light_to_world,
							const MediumInterface& medium_interface,
							unsigned int n_samples /* = 1 */)
    : Light::Light(LightFlags(flags | LIGHT_HAS_AREA), light_to_world, medium_interface, n_samples)
{}

inline AreaLight::AreaLight(const Transformation& light_to_world,
							const MediumInterface& medium_interface,
							unsigned int n_samples /* = 1 */)
    : Light::Light(LightFlags(LIGHT_HAS_AREA), light_to_world, medium_interface, n_samples)
{}

// VisibilityTester inline method definitions

inline VisibilityTester::VisibilityTester()
    : start_point(), end_point()
{}

inline VisibilityTester::VisibilityTester(const ScatteringEvent& start_point,
										  const ScatteringEvent& end_point)
    : start_point(start_point), end_point(end_point)
{}

inline const ScatteringEvent& VisibilityTester::startPoint() const
{
    return start_point;
}

inline const ScatteringEvent& VisibilityTester::endPoint() const
{
    return end_point;
}

} // RayImpact
} // Impact
