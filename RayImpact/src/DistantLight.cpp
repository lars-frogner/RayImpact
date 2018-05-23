#include "DistantLight.hpp"
#include "Scene.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// DistantLight method definitions

void DistantLight::preprocess(const Scene& scene)
{
    scene.worldSpaceBoundingBox().boundingSphere(&scene_center, &scene_radius);
}

RadianceSpectrum DistantLight::sampleIncidentRadiance(const ScatteringEvent& scattering_event,
                                                      const Point2F& uniform_sample,
                                                      Vector3F* incident_direction,
                                                      imp_float* pdf_value,
                                                      VisibilityTester* visibility_tester) const
{
    *incident_direction = -direction;

    *pdf_value = 1.0f;

    const Point3F& outside_point = scattering_event.position - direction*(2*scene_radius);

    *visibility_tester = VisibilityTester(ScatteringEvent(outside_point, medium_interface, scattering_event.time), scattering_event);

    return incident_radiance;
}

// DistantLight function definitions

std::shared_ptr<Light> createDistantLight(const Transformation& light_to_world,
                                          const MediumInterface& medium_interface,
                                          const ParameterSet& parameters)
{
    const Vector3F& direction = parameters.getSingleTripleValue("direction", Vector3F(0, 0, -1)).normalized();
    const RadianceSpectrum& radiance = parameters.getSingleSpectrumValue("radiance", RadianceSpectrum(1.0f));
	
	if (RIMP_OPTIONS.verbosity >= IMP_LIGHTS_VERBOSITY)
	{
		printInfoMessage("Light:"
						 "\n    %-20s%s"
						 "\n    %-20s%s W/sr/m^2"
						 "\n    %-20s%s",
						 "Type:", "Distant",
						 "Radiance:", radiance.toRGBString().c_str(),
						 "Direction:", light_to_world(direction).toString().c_str());
	}

    return std::make_shared<DistantLight>(light_to_world,
                                          direction,
                                          radiance);
}

} // RayImpact
} // Impact
