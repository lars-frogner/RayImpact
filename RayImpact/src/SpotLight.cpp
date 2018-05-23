#include "SpotLight.hpp"
#include "api.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// PointLight method definitions

imp_float SpotLight::falloffInDirection(const Vector3F& direction) const
{
    const Vector3F& direction_in_light_system = world_to_light(direction).normalized();

    imp_float cos_direction_angle = direction_in_light_system.z;

    if (cos_direction_angle < cos_max_angle)
        return 0;

    if (cos_direction_angle >= cos_falloff_start_angle)
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

// SpotLight function definitions

std::shared_ptr<Light> createSpotLight(const Transformation& light_to_world,
                                       const MediumInterface& medium_interface,
                                       const ParameterSet& parameters)
{
    const IntensitySpectrum& intensity = parameters.getSingleSpectrumValue("intensity", RadianceSpectrum(1.0f));
    imp_float cone_width = parameters.getSingleFloatValue("cone_width", 180.0f);
    imp_float falloff_start = parameters.getSingleFloatValue("falloff_start", cone_width);
	
	if (RIMP_OPTIONS.verbosity >= IMP_LIGHTS_VERBOSITY)
	{
		printInfoMessage("Light:"
						 "\n    %-20s%s"
						 "\n    %-20s%s W/sr"
						 "\n    %-20s%g degrees"
						 "\n    %-20s%g degrees"
						 "\n    %-20s%s m"
						 "\n    %-20s%s",
						 "Type:", "Spot",
						 "Intensity:", intensity.toRGBString().c_str(),
						 "Cone width:", cone_width,
						 "Falloff start", falloff_start,
						 "Position:", light_to_world(Point3F(0, 0, 0)).toString().c_str(),
						 "Direction:", light_to_world(Vector3F(0, 0, 1)).toString().c_str());
	}

    return std::make_shared<SpotLight>(light_to_world,
                                       medium_interface,
                                       intensity,
                                       cone_width,
                                       falloff_start);
}

} // RayImpact
} // Impact
