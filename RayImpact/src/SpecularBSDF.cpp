#include "SpecularBSDF.hpp"
#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// SpecularBSDF method definitions

Spectrum SpecularBSDF::sample(const Vector3F& outgoing_direction,
                              Vector3F* incident_direction,
                              const Point2F& uniform_sample,
                              imp_float* pdf_value,
                              BXDFType* sampled_type /* = nullptr */) const
{
	imp_float fresnel_reflectance = fresnelReflectance(cosTheta(outgoing_direction),
													   refractive_index_outside,
													   refractive_index_inside);

	if (uniform_sample.x < fresnel_reflectance)
	{
		*incident_direction = Vector3F(-outgoing_direction.x,
									   -outgoing_direction.y,
										outgoing_direction.z);

		if (sampled_type)
			*sampled_type = BXDFType(BSDF_SPECULAR | BSDF_REFLECTION);

		*pdf_value = fresnel_reflectance;

		return reflectance*fresnel_reflectance/absCosTheta(*incident_direction);
	}
	else
	{
		TransmissionSpectrum fresnel_transmittance = (1 - fresnel_reflectance)*transmittance;

		if (cosTheta(outgoing_direction) > 0)
		{
			if (!refract(outgoing_direction,
						 Normal3F(0, 0, 1),
						 refractive_index_outside,
						 refractive_index_inside,
						 incident_direction))
			{
				return Spectrum(0.0f);
			}

			if (transport_mode == TransportMode::Radiance)
				fresnel_transmittance *= refractive_index_outside*refractive_index_outside/
										 (refractive_index_inside*refractive_index_inside);
		}
		else
		{
			if (!refract(outgoing_direction,
						 Normal3F(0, 0, -1),
						 refractive_index_inside,
						 refractive_index_outside,
						 incident_direction))
			{
				return Spectrum(0.0f);
			}

			if (transport_mode == TransportMode::Radiance)
				fresnel_transmittance *= refractive_index_inside*refractive_index_inside/
										 (refractive_index_outside*refractive_index_outside);
		}

		if (sampled_type)
			*sampled_type = BXDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);

		*pdf_value = 1 - fresnel_reflectance;

		return fresnel_transmittance/absCosTheta(*incident_direction);
	}
}

} // RayImpact
} // Impact
