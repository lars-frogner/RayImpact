#include "BSDF.hpp"
#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// BXDF method definitions

Spectrum BXDF::sample(const Vector3F& outgoing_direction,
                      Vector3F* incident_direction,
                      const Point2F& uniform_sample,
                      imp_float* pdf_value,
                      BXDFType* sampled_type /* = nullptr */) const
{
	*incident_direction = cosineWeightedHemisphereSample(uniform_sample);
	if (outgoing_direction.z < 0) incident_direction->z *= -1;

	*pdf_value = pdf(outgoing_direction, *incident_direction);

    return evaluate(outgoing_direction, *incident_direction);
}

Spectrum BXDF::reduced(const Vector3F& outgoing_direction,
                       unsigned int n_samples,
                       const Point2F* samples) const
{
	Spectrum result(0.0f);

	Vector3F incident_direction;
	imp_float pdf_value;
	Spectrum bsdf_value;

	for (unsigned int i = 0; i < n_samples; i++)
	{
		pdf_value = 0;
		bsdf_value = sample(outgoing_direction, &incident_direction, samples[i], &pdf_value);
		if (pdf_value > 0)
			result += bsdf_value*absCosTheta(incident_direction)/pdf_value;
	}

    return result/static_cast<imp_float>(n_samples);
}

Spectrum BXDF::reduced(unsigned int n_samples,
                       const Point2F* samples_1,
                       const Point2F* samples_2) const
{
    ReflectionSpectrum result(0.0f);

	Vector3F outgoing_direction;
	Vector3F incident_direction;
	imp_float outgoing_pdf_value;
	imp_float incident_pdf_value;
	Spectrum bsdf_value;

	for (unsigned int i = 0; i < n_samples; i++)
	{
		outgoing_direction = uniformHemisphereSample(samples_1[i]);
		outgoing_pdf_value = IMP_ONE_OVER_TWO_PI;
		incident_pdf_value = 0;

		bsdf_value = sample(outgoing_direction, &incident_direction, samples_2[i], &incident_pdf_value);

		if (incident_pdf_value > 0)
			result += bsdf_value*absCosTheta(incident_direction)*absCosTheta(outgoing_direction)/(outgoing_pdf_value*incident_pdf_value);
	}

    return result/(IMP_PI*n_samples);
}

// BSDF method definitions

unsigned int BSDF::numberOfComponents(BXDFType type /* = BSDF_ALL */) const
{
    if (type == BSDF_ALL)
        return n_bxdfs;

    unsigned int n = 0;

    for (unsigned int i = 0; i < n_bxdfs; i++)
    {
        if (bxdfs[i]->containedIn(type))
            n++;
    }

    return n;
}

Spectrum BSDF::evaluate(const Vector3F& world_outgoing_direction,
                        const Vector3F& world_incident_direction,
                        BXDFType type /* = BSDF_ALL */) const
{
    bool is_reflection = world_outgoing_direction.dot(geometric_normal)*world_incident_direction.dot(geometric_normal) > 0;

    const Vector3F& outgoing_direction = worldToLocal(world_outgoing_direction);
    const Vector3F& incident_direction = worldToLocal(world_incident_direction);

    Spectrum result(0.0f);

    for (unsigned int i = 0; i < n_bxdfs; i++)
    {
        if (bxdfs[i]->containedIn(type) &&
            (( is_reflection && bxdfs[i]->contains(BSDF_REFLECTION)) ||
             (!is_reflection && bxdfs[i]->contains(BSDF_TRANSMISSION))))
        {
            result += bxdfs[i]->evaluate(outgoing_direction, incident_direction);
        }
    }

    return result;
}

Spectrum BSDF::reduced(const Vector3F& outgoing_direction,
                       unsigned int n_samples,
                       const Point2F* samples,
                       BXDFType type /* = BSDF_ALL */) const
{
    Spectrum result(0.0f);

    for (unsigned int i = 0; i < n_bxdfs; i++)
    {
        if (bxdfs[i]->containedIn(type))
            result += bxdfs[i]->reduced(outgoing_direction, n_samples, samples);
    }

    return result;
}

Spectrum BSDF::reduced(unsigned int n_samples,
                       const Point2F* samples_1,
                       const Point2F* samples_2,
                       BXDFType type /* = BSDF_ALL */) const
{
    Spectrum result(0.0f);

    for (unsigned int i = 0; i < n_bxdfs; i++)
    {
        if (bxdfs[i]->containedIn(type))
            result += bxdfs[i]->reduced(n_samples, samples_1, samples_2);
    }

    return result;
}

Spectrum BSDF::sample(const Vector3F& world_outgoing_direction,
                      Vector3F* world_incident_direction,
                      const Point2F& uniform_sample,
                      imp_float* pdf_value,
                      BXDFType type /* = BSDF_ALL */,
					  BXDFType* sampled_type /* = nullptr */) const
{
	*pdf_value = 0;

    unsigned int n_matching_components = numberOfComponents(type);

	if (n_matching_components == 0)
		return Spectrum(0.0f);

	// Use first sample value to choose one of the matching BxDF components
	int component = std::min<int>(static_cast<int>(std::floor(uniform_sample.x*n_matching_components)),
								  n_matching_components - 1);

	BXDF* bxdf = nullptr;
	int count = component;

	// Find the pointer to the matching component
	for (unsigned int i = 0; i < n_bxdfs; i++)
	{
		if (bxdfs[i]->containedIn(type) && count-- == 0)
		{
			bxdf = bxdfs[i];
			break;
		}
	}

	imp_assert(bxdf);

	// Remap the first sample value to ensure that it is uniformly distributed
	Point2F remapped_uniform_sample(n_matching_components*uniform_sample.x - count, uniform_sample.y);

	Vector3F incident_direction;
	const Vector3F& outgoing_direction = worldToLocal(world_outgoing_direction);

	if (sampled_type)
		*sampled_type = bxdf->type;

	Spectrum bsdf_value = bxdf->sample(outgoing_direction,
									   &incident_direction,
									   remapped_uniform_sample,
									   pdf_value,
									   sampled_type);

	if (*pdf_value == 0)
		return Spectrum(0.0f);

	*world_incident_direction = localToWorld(incident_direction);

	if (!(bxdf->contains(BSDF_SPECULAR)) && n_matching_components > 1)
	{
		for (unsigned int i = 0; i < n_bxdfs; i++)
		{
			if (bxdfs[i] != bxdf && bxdfs[i]->containedIn(type))
				*pdf_value += bxdfs[i]->pdf(outgoing_direction, incident_direction);
		}
	}

	if (n_matching_components > 1)
		*pdf_value /= n_matching_components;

	if (!(bxdf->contains(BSDF_SPECULAR)) && n_matching_components > 1)
	{
		bool was_reflected = world_incident_direction->dot(geometric_normal)*world_outgoing_direction.dot(geometric_normal) > 0;
		bsdf_value = 0.0f;

		for (unsigned int i = 0; i < n_bxdfs; i++)
		{
			if (bxdfs[i]->containedIn(type) &&
				(( was_reflected && bxdfs[i]->contains(BSDF_REFLECTION)) ||
				 (!was_reflected && bxdfs[i]->contains(BSDF_TRANSMISSION))))
				bsdf_value += bxdfs[i]->evaluate(outgoing_direction, incident_direction);
		}
	}

	return bsdf_value;
}

imp_float BSDF::pdf(const Vector3F& outgoing_direction,
                    const Vector3F& incident_direction,
                    BXDFType type /* = BSDF_ALL */) const
{
	imp_float pdf_value = 0;

	for (unsigned int i = 0; i < n_bxdfs; i++)
	{
		if (bxdfs[i]->containedIn(type))
			pdf_value += bxdfs[i]->pdf(outgoing_direction, incident_direction);
	}

	pdf_value /= n_bxdfs;

	return pdf_value;
}

} // RayImpact
} // Impact
