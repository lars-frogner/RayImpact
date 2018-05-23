#include "DiffuseAreaLight.hpp"
#include "api.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// DiffuseAreaLight function definitions

std::shared_ptr<AreaLight> createDiffuseAreaLight(const Transformation& light_to_world,
                                                  const MediumInterface& medium_interface,
                                                  const ParameterSet& parameters,
                                                  std::shared_ptr<Shape> shape)
{
    const RadianceSpectrum& radiance = parameters.getSingleSpectrumValue("radiance", RadianceSpectrum(1.0f));
    unsigned int samples = (unsigned int)std::abs(parameters.getSingleIntValue("samples", 1));
	
	if (RIMP_OPTIONS.verbosity >= IMP_LIGHTS_VERBOSITY)
	{
		printInfoMessage("Area light:"
						 "\n    %-20s%s"
						 "\n    %-20s%s W/sr/m^2"
						 "\n    %-20s%u",
						 "Type:", "Diffuse",
						 "Radiance:", radiance.toRGBString().c_str(),
						 "Samples:", samples);
	}

    return std::make_shared<DiffuseAreaLight>(light_to_world,
                                              medium_interface,
                                              radiance,
                                              samples,
                                              shape);
}

} // RayImpact
} // Impact
