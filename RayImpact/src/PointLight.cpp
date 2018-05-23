#include "PointLight.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// PointLight method definitions

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

// PointLight function definitions

std::shared_ptr<Light> createPointLight(const Transformation& light_to_world,
                                        const MediumInterface& medium_interface,
                                        const ParameterSet& parameters)
{
    const IntensitySpectrum& intensity = parameters.getSingleSpectrumValue("intensity", RadianceSpectrum(1.0f));
	
	if (RIMP_OPTIONS.verbosity >= IMP_LIGHTS_VERBOSITY)
	{
		printInfoMessage("Light:"
						 "\n    %-20s%s"
						 "\n    %-20s%s W/sr"
						 "\n    %-20s%s m",
						 "Type:", "Point",
						 "Intensity:", intensity.toRGBString().c_str(),
						 "Position:", light_to_world(Point3F(0, 0, 0)).toString().c_str());
	}

    return std::make_shared<PointLight>(light_to_world,
                                        medium_interface,
                                        intensity);
}

} // RayImpact
} // Impact
