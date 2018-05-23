#include "MicrofacetBTDF.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetBTDF method definitions

Spectrum MicrofacetBTDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    if (sameHemisphere(outgoing_direction, incident_direction))
        return Spectrum(0.0f);

    imp_float cos_theta_outgoing = cosTheta(outgoing_direction);
    imp_float cos_theta_incident = cosTheta(incident_direction);

    if (cos_theta_incident == 0 || cos_theta_outgoing == 0)
        return Spectrum(0.0f);

    imp_float refractive_index_ratio = (cosTheta(outgoing_direction) > 0)? refractive_index_inside/refractive_index_outside :
                                                                           refractive_index_outside/refractive_index_inside;

    Vector3F half_vector = (outgoing_direction + incident_direction*refractive_index_ratio).normalized();

    if (half_vector.z < 0)
        half_vector.reverse();

    imp_float outgoing_dot_half = outgoing_direction.dot(half_vector);
    imp_float incident_dot_half = incident_direction.dot(half_vector);

    imp_float denom_factor = outgoing_dot_half + refractive_index_ratio*incident_dot_half;

    imp_float num_factor = (transport_mode == TransportMode::Radiance)? 1.0f : refractive_index_ratio*refractive_index_ratio;

    return transmittance*
           (Spectrum(1.0f) - dielectric_reflector.evaluate(outgoing_dot_half))*
           (microfacet_distribution->areaWithMicroNormal(half_vector)*
            microfacet_distribution->visibleFraction(outgoing_direction, incident_direction)*
            num_factor*std::abs(outgoing_dot_half)*std::abs(incident_dot_half)/
            (denom_factor*denom_factor*cos_theta_incident*cos_theta_outgoing));
}

Spectrum MicrofacetBTDF::sample(const Vector3F& outgoing_direction,
								Vector3F* incident_direction,
								const Point2F& uniform_sample,
								imp_float* pdf_value,
								BXDFType* sampled_type /* = nullptr */) const
{
	const Vector3F& micro_normal = microfacet_distribution->sampleMicroNormal(outgoing_direction, uniform_sample);
    imp_float refractive_index_ratio = (cosTheta(outgoing_direction) > 0)? refractive_index_inside/refractive_index_outside :
                                                                           refractive_index_outside/refractive_index_inside;

	if (!refract(outgoing_direction,
				 static_cast<Normal3F>(micro_normal),
				 refractive_index_ratio,
				 incident_direction))
		return Spectrum(0.0f);

	*pdf_value = pdf(outgoing_direction, *incident_direction);

	return evaluate(outgoing_direction, *incident_direction);
}

imp_float MicrofacetBTDF::pdf(const Vector3F& outgoing_direction,
							  const Vector3F& incident_direction) const
{
	if (!sameHemisphere(outgoing_direction, incident_direction))
		return 0;
	
    imp_float refractive_index_ratio = (cosTheta(outgoing_direction) > 0)? refractive_index_inside/refractive_index_outside :
                                                                           refractive_index_outside/refractive_index_inside;

	const Vector3F& micro_normal = (outgoing_direction + incident_direction*refractive_index_ratio).normalized();
	
	imp_float cos_theta_incident = incident_direction.dot(micro_normal);
	imp_float sqrt_denom = outgoing_direction.dot(micro_normal) + refractive_index_ratio*cos_theta_incident;

	// Apply change of variables from micro normal to incident direction for pdf
	return microfacet_distribution->pdf(outgoing_direction, micro_normal)*
		   std::abs((refractive_index_ratio*refractive_index_ratio*cos_theta_incident)/
					(sqrt_denom*sqrt_denom));
}

} // RayImpact
} // Impact
