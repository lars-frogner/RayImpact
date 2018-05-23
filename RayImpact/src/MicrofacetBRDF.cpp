#include "MicrofacetBRDF.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetBRDF method definitions

Spectrum MicrofacetBRDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    imp_float cos_theta_outgoing = absCosTheta(outgoing_direction);
    imp_float cos_theta_incident = absCosTheta(incident_direction);

    Vector3F half_vector = outgoing_direction + incident_direction;

    if (cos_theta_incident == 0 || cos_theta_outgoing == 0 || !half_vector.nonZero())
        return Spectrum(0.0f);

    half_vector.normalize();

    return reflectance*
           fresnel_reflector->evaluate(outgoing_direction.dot(half_vector))*
           (microfacet_distribution->areaWithMicroNormal(half_vector)*
            microfacet_distribution->visibleFraction(outgoing_direction, incident_direction)/
            (4*cos_theta_incident*cos_theta_outgoing));
}

Spectrum MicrofacetBRDF::sample(const Vector3F& outgoing_direction,
								Vector3F* incident_direction,
								const Point2F& uniform_sample,
								imp_float* pdf_value,
								BXDFType* sampled_type /* = nullptr */) const
{
	const Vector3F& micro_normal = microfacet_distribution->sampleMicroNormal(outgoing_direction, uniform_sample);
	*incident_direction = outgoing_direction.reflectedAbout(micro_normal);

	if (!sameHemisphere(outgoing_direction, *incident_direction))
		return Spectrum(0.0f);

	*pdf_value = microfacet_distribution->pdf(outgoing_direction, micro_normal)/(4*outgoing_direction.dot(micro_normal));

	return evaluate(outgoing_direction, *incident_direction);
}

imp_float MicrofacetBRDF::pdf(const Vector3F& outgoing_direction,
							  const Vector3F& incident_direction) const
{
	if (!sameHemisphere(outgoing_direction, incident_direction))
		return 0;
	const Vector3F& micro_normal = (outgoing_direction + incident_direction).normalized();
	return microfacet_distribution->pdf(outgoing_direction, micro_normal)/(4*outgoing_direction.dot(micro_normal));
}

} // RayImpact
} // Impact
