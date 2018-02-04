#include "BSDF.hpp"
#include "error.hpp"
#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// BXDF method implementations

BXDF::BXDF(BXDFType type)
    : type(type)
{}

bool BXDF::containedIn(BXDFType t) const
{
    return (type & t) == type;
}

bool BXDF::contains(BXDFType t) const
{
    return type & t;
}

Spectrum BXDF::sample(const Vector3F& outgoing_direction,
                      Vector3F* incident_direction,
                      const Point2F& sample_values,
                      imp_float* pdf_value,
                      BXDFType* sampled_type /* = nullptr */) const
{
    printSevereMessage("BXDF::sample was called without being implemented");
    return Spectrum(0.0f);
}

Spectrum BXDF::reduced(const Vector3F& outgoing_direction,
                       unsigned int n_samples,
                       const Point2F* samples) const
{
    printSevereMessage("BXDF::reduced (1) was called without being implemented");
    return Spectrum(0.0f);
}

Spectrum BXDF::reduced(unsigned int n_samples,
                       const Point2F* samples_1,
                       const Point2F* samples_2) const
{
    printSevereMessage("BXDF::reduced (2) was called without being implemented");
    return Spectrum(0.0f);
}

imp_float BXDF::pdf(const Vector3F& outgoing_direction,
                    const Vector3F& incident_direction) const
{
    printSevereMessage("BXDF::pdf was called without being implemented");
    return 0.0f;
}

// ScaledBXDF method implementations

ScaledBXDF::ScaledBXDF(BXDF* bxdf, const Spectrum& scale)
    : BXDF::BXDF(BXDFType(bxdf->type)),
      bxdf(bxdf),
      scale(scale)
{}

Spectrum ScaledBXDF::evaluate(const Vector3F& outgoing_direction,
                              const Vector3F& incident_direction) const
{
    return scale*bxdf->evaluate(outgoing_direction, incident_direction);
}

Spectrum ScaledBXDF::sample(const Vector3F& outgoing_direction,
                            Vector3F* incident_direction,
                            const Point2F& sample_values,
                            imp_float* pdf_value,
                            BXDFType* sampled_type /* = nullptr */) const
{
    return scale*bxdf->sample(outgoing_direction,
                              incident_direction,
                              sample_values,
                              pdf_value,
                              sampled_type);
}

Spectrum ScaledBXDF::reduced(const Vector3F& outgoing_direction,
                             unsigned int n_samples,
                             const Point2F* samples) const
{
    return scale*bxdf->reduced(outgoing_direction, n_samples, samples);
}

Spectrum ScaledBXDF::reduced(unsigned int n_samples,
                             const Point2F* samples_1,
                             const Point2F* samples_2) const
{
    return scale*bxdf->reduced(n_samples, samples_1, samples_2);
}

imp_float ScaledBXDF::pdf(const Vector3F& outgoing_direction,
                          const Vector3F& incident_direction) const
{
    return bxdf->pdf(outgoing_direction, incident_direction);
}

// SpecularBRDF method implementations

SpecularBRDF::SpecularBRDF(const ReflectionSpectrum& reflectance,
                           FresnelReflector* fresnel_reflector)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
      reflectance(reflectance),
      fresnel_reflector(fresnel_reflector)
{}

Spectrum SpecularBRDF::evaluate(const Vector3F& outgoing_direction,
                                const Vector3F& incident_direction) const
{
    return Spectrum(0.0f);
}

Spectrum SpecularBRDF::sample(const Vector3F& outgoing_direction,
                              Vector3F* incident_direction,
                              const Point2F& sample_values,
                              imp_float* pdf_value,
                              BXDFType* sampled_type /* = nullptr */) const
{
    *incident_direction = Vector3F(-outgoing_direction.x, -outgoing_direction.y, outgoing_direction.z);

    *pdf_value = 1.0f;

    return reflectance*fresnel_reflector->evaluate(cosTheta(*incident_direction))/absCosTheta(*incident_direction);
}

// SpecularBTDF method implementations

SpecularBTDF::SpecularBTDF(const TransmissionSpectrum& transmittance,
                           imp_float refractive_index_outside,
                           imp_float refractive_index_inside,
                           TransportMode transport_mode)
    : BXDF::BXDF(BXDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)),
      transmittance(transmittance),
      refractive_index_outside(refractive_index_outside),
      refractive_index_inside(refractive_index_inside),
      dielectric_reflector(refractive_index_outside,
                           refractive_index_inside),
      transport_mode(transport_mode)
{}

Spectrum SpecularBTDF::evaluate(const Vector3F& outgoing_direction,
                                const Vector3F& incident_direction) const
{
    return Spectrum(0.0f);
}

Spectrum SpecularBTDF::sample(const Vector3F& outgoing_direction,
                              Vector3F* incident_direction,
                              const Point2F& sample_values,
                              imp_float* pdf_value,
                              BXDFType* sampled_type /* = nullptr */) const
{
    *pdf_value = 1.0f;

    Spectrum result;

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

        result = transmittance*(Spectrum(1.0f) - dielectric_reflector.evaluate(cosTheta(*incident_direction)));

        if (transport_mode == TransportMode::Radiance)
            result *= refractive_index_outside*refractive_index_outside/(refractive_index_inside*refractive_index_inside);
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

        result = transmittance*(Spectrum(1.0f) - dielectric_reflector.evaluate(cosTheta(*incident_direction)));

        if (transport_mode == TransportMode::Radiance)
            result *= refractive_index_inside*refractive_index_inside/(refractive_index_outside*refractive_index_outside);
    }

    return result/absCosTheta(*incident_direction);
}

// SpecularBSDF method implementations

SpecularBSDF::SpecularBSDF(const ReflectionSpectrum& reflectance,
                           const TransmissionSpectrum& transmittance,
                           imp_float refractive_index_outside,
                           imp_float refractive_index_inside,
                           TransportMode transport_mode)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
      reflectance(reflectance),
      transmittance(transmittance),
      refractive_index_outside(refractive_index_outside),
      refractive_index_inside(refractive_index_inside),
      dielectric_reflector(refractive_index_outside,
                           refractive_index_inside),
      transport_mode(transport_mode)
{}

Spectrum SpecularBSDF::evaluate(const Vector3F& outgoing_direction,
                                const Vector3F& incident_direction) const
{
    return Spectrum(0.0f);
}

Spectrum SpecularBSDF::sample(const Vector3F& outgoing_direction,
                              Vector3F* incident_direction,
                              const Point2F& sample_values,
                              imp_float* pdf_value,
                              BXDFType* sampled_type /* = nullptr */) const
{
    printSevereMessage("SpecularBSDF::sample is not implemented");
    return Spectrum(0.0f);
}

// LambertianBRDF method implementations

LambertianBRDF::LambertianBRDF(const ReflectionSpectrum& reflectance)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
      reflectance(reflectance)
{}

Spectrum LambertianBRDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    return reflectance*IMP_ONE_OVER_PI;
}

Spectrum LambertianBRDF::sample(const Vector3F& outgoing_direction,
                                Vector3F* incident_direction,
                                const Point2F& sample_values,
                                imp_float* pdf_value,
                                BXDFType* sampled_type /* = nullptr */) const
{
    *incident_direction = cosineWeightedHemisphereSample(sample_values);

    if (outgoing_direction.z < 0)
        incident_direction->z = -incident_direction->z;

    *pdf_value = pdf(outgoing_direction, *incident_direction);

    return evaluate(outgoing_direction, *incident_direction);
}

Spectrum LambertianBRDF::reduced(const Vector3F& outgoing_direction,
                                 unsigned int n_samples,
                                 const Point2F* samples) const
{
    return reflectance;
}

Spectrum LambertianBRDF::reduced(unsigned int n_samples,
                                 const Point2F* samples_1,
                                 const Point2F* samples_2) const
{
    return reflectance;
}

imp_float LambertianBRDF::pdf(const Vector3F& outgoing_direction,
                              const Vector3F& incident_direction) const
{
    return (sameHemisphere(outgoing_direction, incident_direction))? absCosTheta(incident_direction)*IMP_ONE_OVER_PI : 0;
}

// LambertianBTDF method implementations

LambertianBTDF::LambertianBTDF(const TransmissionSpectrum& transmittance)
    : BXDF::BXDF(BXDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)),
      transmittance(transmittance)
{}

Spectrum LambertianBTDF::evaluate(const Vector3F& outgoing_direction,
                                  const Vector3F& incident_direction) const
{
    return transmittance*IMP_ONE_OVER_PI;
}

Spectrum LambertianBTDF::sample(const Vector3F& outgoing_direction,
                                Vector3F* incident_direction,
                                const Point2F& sample_values,
                                imp_float* pdf_value,
                                BXDFType* sampled_type /* = nullptr */) const
{
    *incident_direction = cosineWeightedHemisphereSample(sample_values);

    if (outgoing_direction.z > 0)
        incident_direction->z = -incident_direction->z;

    *pdf_value = pdf(outgoing_direction, *incident_direction);

    return evaluate(outgoing_direction, *incident_direction);
}

imp_float LambertianBTDF::pdf(const Vector3F& outgoing_direction,
                              const Vector3F& incident_direction) const
{
    return (!sameHemisphere(outgoing_direction, incident_direction))? absCosTheta(incident_direction)*IMP_ONE_OVER_PI : 0;
}

// LambertianBTDF method implementations

OrenNayarBRDF::OrenNayarBRDF(const ReflectionSpectrum& reflectance,
                             imp_float slope_deviation)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
      reflectance(reflectance)
{
    imp_float sigma_rad = degreesToRadians(slope_deviation);
    imp_float sigma_rad_sq = sigma_rad*sigma_rad;

    A = 1 - sigma_rad_sq/(2*(sigma_rad_sq + 0.33f));
    B = 0.45f*sigma_rad_sq/(sigma_rad_sq + 0.09f);
}

Spectrum OrenNayarBRDF::evaluate(const Vector3F& outgoing_direction,
                                 const Vector3F& incident_direction) const
{
    imp_float sin_theta_outgoing = sinTheta(outgoing_direction);
    imp_float sin_theta_incident = sinTheta(incident_direction);

    imp_float cos_delta_phi = 0;

    if (sin_theta_outgoing > 1e-4 && sin_theta_incident > 1e-4)
    {
        cos_delta_phi = std::max<imp_float>(0, cosPhi(incident_direction)*cosPhi(outgoing_direction) +
                                               sinPhi(incident_direction)*sinPhi(outgoing_direction));
    }

    imp_float sin_alpha, tan_beta;

    if (absCosTheta(incident_direction) > absCosTheta(outgoing_direction))
    {
        // theta_outgoing is larger than theta_incident
        sin_alpha = sin_theta_outgoing;
        tan_beta = sin_theta_incident/absCosTheta(incident_direction);
    }
    else
    {
        // theta_incident is larger than theta_outgoing
        sin_alpha = sin_theta_incident;
        tan_beta = sin_theta_outgoing/absCosTheta(outgoing_direction);
    }

    return reflectance*IMP_ONE_OVER_PI*(A + B*cos_delta_phi*sin_alpha*tan_beta);
}

// MicrofacetBRDF method implementations

MicrofacetBRDF::MicrofacetBRDF(const ReflectionSpectrum& reflectance,
                               MicrofacetDistribution* microfacet_distribution,
                               FresnelReflector* fresnel_reflector)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
      reflectance(reflectance),
      microfacet_distribution(microfacet_distribution),
      fresnel_reflector(fresnel_reflector)
{}

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

// MicrofacetBTDF method implementations

MicrofacetBTDF::MicrofacetBTDF(const TransmissionSpectrum& transmittance,
                               imp_float refractive_index_outside,
                               imp_float refractive_index_inside,
                               MicrofacetDistribution* microfacet_distribution,
                               TransportMode transport_mode)
    : BXDF::BXDF(BXDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
      transmittance(transmittance),
      refractive_index_outside(refractive_index_outside),
      refractive_index_inside(refractive_index_inside),
      microfacet_distribution(microfacet_distribution),
      dielectric_reflector(refractive_index_outside,
                           refractive_index_inside),
      transport_mode(transport_mode)
{}

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

// BSDF method implementations

BSDF::BSDF(const SurfaceScatteringEvent& scattering_event,
           imp_float refractive_index_outside /* = 1.0f */)
    : refractive_index_outside(refractive_index_outside),
      geometric_normal(scattering_event.surface_normal),
      shading_normal(scattering_event.shading.surface_normal),
      shading_tangent(scattering_event.shading.dpdu.normalized()),
      shading_bitangent(Vector3F(shading_normal).cross(shading_tangent))
{}

void BSDF::addComponent(BXDF* bxdf)
{
    imp_assert(n_bxdfs < max_bxdfs);
    bxdfs[n_bxdfs++] = bxdf;
}

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

Vector3F BSDF::worldToLocal(const Vector3F& vector) const
{
    return Vector3F(shading_tangent.dot(vector), shading_bitangent.dot(vector), shading_normal.dot(vector));
}

Vector3F BSDF::localToWorld(const Vector3F& vector) const
{
    return Vector3F(shading_tangent.x*vector.x + shading_bitangent.x*vector.y + shading_normal.x*vector.z,
                    shading_tangent.y*vector.x + shading_bitangent.y*vector.y + shading_normal.y*vector.z,
                    shading_tangent.z*vector.x + shading_bitangent.z*vector.y + shading_normal.z*vector.z);
}

Spectrum BSDF::evaluate(const Vector3F& world_outgoing_direction,
                        const Vector3F& world_incident_direction,
                        BXDFType type) const
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

// BSDF utility functions

// Computes Fresnel reflectance for dielectric materials
imp_float fresnelReflectance(imp_float cos_incident_angle,
                             imp_float refractive_index_outside,
                             imp_float refractive_index_inside)
{
    // Make sure that the incident angle is valid
    cos_incident_angle = clamp(cos_incident_angle, -1.0f, 1.0f);

    imp_float refractive_index_incident_medium;
    imp_float refractive_index_transmitted_medium;

    // Check whether the ray is entering the object from the outside
    if (cos_incident_angle > 0)
    {
        refractive_index_incident_medium = refractive_index_outside;
        refractive_index_transmitted_medium = refractive_index_inside;
    }
    else
    {
        refractive_index_incident_medium = refractive_index_inside;
        refractive_index_transmitted_medium = refractive_index_outside;

        // The incident angle is now with respect to the normal in the incident direction
        cos_incident_angle = std::abs(cos_incident_angle);
    }

    imp_float sin_incident_angle = std::sqrt(std::max<imp_float>(0, 1 - cos_incident_angle*cos_incident_angle));

    // Compute transmitted angle with Snells' law
    imp_float sin_transmitted_angle = (refractive_index_incident_medium/refractive_index_transmitted_medium)*sin_incident_angle;

    // Total internal reflection
    if (sin_transmitted_angle >= 1)
        return 1.0f;

    imp_float cos_transmitted_angle = std::sqrt(std::max<imp_float>(0, 1 - sin_transmitted_angle*sin_transmitted_angle));

    // Compute Fresnel reflectance for parallel polarized light
    imp_float reflectance_parallel = ((refractive_index_transmitted_medium*cos_incident_angle) -
                                      (refractive_index_incident_medium*cos_transmitted_angle))/
                                     ((refractive_index_transmitted_medium*cos_incident_angle) +
                                      (refractive_index_incident_medium*cos_transmitted_angle));

    // Compute Fresnel reflectance for perpendicular polarized light
    imp_float reflectance_perpendicular = ((refractive_index_incident_medium*cos_incident_angle) -
                                           (refractive_index_transmitted_medium*cos_transmitted_angle))/
                                          ((refractive_index_incident_medium*cos_incident_angle) +
                                           (refractive_index_transmitted_medium*cos_transmitted_angle));

    // For unpolarized light the Fresnel reflectance is the average of the squares of the two polarization components
    return 0.5f*(reflectance_parallel*reflectance_parallel + reflectance_perpendicular*reflectance_perpendicular);
}

// Computes Fresnel reflectance for conducting materials
ReflectionSpectrum fresnelReflectance(imp_float cos_incident_angle,
                                      const Spectrum& refractive_index_outside,
                                      const Spectrum& refractive_index_inside,
                                      const Spectrum& absorption_coefficient_inside)
{
    cos_incident_angle = clamp(cos_incident_angle, -1.0f, 1.0f);

    imp_float cos_sq_theta = cos_incident_angle*cos_incident_angle;
    imp_float sin_sq_theta = 1 - cos_sq_theta;

    Spectrum eta_rel = refractive_index_inside/refractive_index_outside;
    Spectrum k_rel = absorption_coefficient_inside/refractive_index_outside;

    Spectrum eta_rel_sq = eta_rel*eta_rel;
    Spectrum k_rel_sq = k_rel*k_rel;

    Spectrum c1 = eta_rel_sq - k_rel_sq - sin_sq_theta;

    Spectrum a_sq_plus_b_sq = sqrt(c1*c1 + 4.0f*eta_rel_sq*k_rel_sq);
    Spectrum a = sqrt(0.5f*(a_sq_plus_b_sq + c1));

    Spectrum c2 = a_sq_plus_b_sq + cos_sq_theta;
    Spectrum c3 = (2.0f*cos_incident_angle)*a;
    Spectrum c4 = cos_sq_theta*a_sq_plus_b_sq + sin_sq_theta*sin_sq_theta;
    Spectrum c5 = sin_sq_theta*c3;

    Spectrum R_perp = (c2 - c3)/(c2 + c3);
    Spectrum R_par = R_perp*(c4 - c5)/(c4 + c5);

    return 0.5f*(R_par*R_par + R_perp*R_perp);
}

bool refract(const Vector3F& incident_direction,
             const Normal3F& incident_surface_normal,
             imp_float refractive_index_incident_medium,
             imp_float refractive_index_transmitted_medium,
             Vector3F* transmitted_direction)
{
    imp_float refractive_index_ratio = refractive_index_incident_medium/refractive_index_transmitted_medium;

    imp_float cos_incident_angle = incident_direction.dot(incident_surface_normal);
    imp_float sin_sq_incident_angle = std::max<imp_float>(0, 1 - cos_incident_angle*cos_incident_angle);
    imp_float sin_sq_transmitted_angle = refractive_index_ratio*refractive_index_ratio*sin_sq_incident_angle;

    // Total internal reflection
    if (sin_sq_transmitted_angle >= 1)
        return false;

    imp_float cos_transmitted_angle = std::sqrt(1 - sin_sq_transmitted_angle);

    *transmitted_direction = (refractive_index_ratio*cos_incident_angle - cos_transmitted_angle)*Vector3F(incident_surface_normal) -
                             refractive_index_ratio*incident_direction;

    return true;
}

} // RayImpact
} // Impact
