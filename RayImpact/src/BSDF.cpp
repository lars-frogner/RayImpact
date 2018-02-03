#include "BSDF.hpp"
#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// BXDF method implementations
    
BXDF::BXDF(BXDFType type)
    : type(type)
{}

bool BXDF::hasType(BXDFType t) const
{
    return (type & t) == type;
}

Spectrum BXDF::sample(const Vector3F& outgoing_dir,
                      Vector3F* incident_dir,
                      const Point2F& sample_point,
                      imp_float* pdf_value,
                      BXDFType* sampled_type = nullptr) const
{
    printSevereMessage("BXDF::sample was called without being implemented");
    return Spectrum(0.0f);
}

Spectrum BXDF::reduced(const Vector3F& outgoing_dir,
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

imp_float BXDF::pdf(const Vector3F& outgoing_dir,
                    const Vector3F& incident_dir) const
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

Spectrum ScaledBXDF::evaluate(const Vector3F& outgoing_dir,
                              const Vector3F& incident_dir) const
{
    return scale*bxdf->evaluate(outgoing_dir, incident_dir);
}

Spectrum ScaledBXDF::sample(const Vector3F& outgoing_dir,
                            Vector3F* incident_dir,
                            const Point2F& sample_point,
                            imp_float* pdf_value,
                            BXDFType* sampled_type = nullptr) const
{
    return scale*bxdf->sample(outgoing_dir,
                              incident_dir,
                              sample_point,
                              pdf_value,
                              sampled_type);
}

Spectrum ScaledBXDF::reduced(const Vector3F& outgoing_dir,
                             unsigned int n_samples,
                             const Point2F* samples) const
{
    return scale*bxdf->reduced(outgoing_dir, n_samples, samples);
}

Spectrum ScaledBXDF::reduced(unsigned int n_samples,
                             const Point2F* samples_1,
                             const Point2F* samples_2) const
{
    return scale*bxdf->reduced(n_samples, samples_1, samples_2);
}

imp_float ScaledBXDF::pdf(const Vector3F& outgoing_dir,
                          const Vector3F& incident_dir) const
{
    return bxdf->pdf(outgoing_dir, incident_dir);
}

// SpecularBRDF method implementations

SpecularBRDF::SpecularBRDF(const ReflectanceSpectrum& reflectance,
                           FresnelReflector* fresnel_reflector)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
      reflectance(reflectance),
      fresnel_reflector(fresnel_reflector)
{}

Spectrum SpecularBRDF::evaluate(const Vector3F& outgoing_dir,
                                const Vector3F& incident_dir) const
{
    return Spectrum(0.0f);
}

Spectrum SpecularBRDF::sample(const Vector3F& outgoing_dir,
                              Vector3F* incident_dir,
                              const Point2F& sample_point,
                              imp_float* pdf_value,
                              BXDFType* sampled_type = nullptr) const
{
    *incident_dir = Vector3F(-outgoing_dir.x, -outgoing_dir.y, outgoing_dir.z);

    *pdf_value = 1.0f;

    return reflectance*fresnel_reflector->evaluate(cosTheta(*incident_dir))/absCosTheta(*incident_dir);
}

// SpecularBTDF method implementations

SpecularBTDF::SpecularBTDF(const TransmittanceSpectrum& transmittance,
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

Spectrum SpecularBTDF::evaluate(const Vector3F& outgoing_dir,
                                const Vector3F& incident_dir) const
{
    return Spectrum(0.0f);
}

Spectrum SpecularBTDF::sample(const Vector3F& outgoing_dir,
                              Vector3F* incident_dir,
                              const Point2F& sample_point,
                              imp_float* pdf_value,
                              BXDFType* sampled_type = nullptr) const
{
    *pdf_value = 1.0f;

    Spectrum result;

    if (cosTheta(outgoing_dir) > 0)
    {
        if (!refract(outgoing_dir,
                     Normal3F(0, 0, 1),
                     refractive_index_outside,
                     refractive_index_inside,
                     incident_dir))
        {
            return Spectrum(0.0f);
        }

        result = transmittance*(Spectrum(1.0f) - dielectric_reflector.evaluate(cosTheta(*incident_dir)));

        if (transport_mode == TransportMode::Radiance)
            result *= refractive_index_outside*refractive_index_outside/(refractive_index_inside*refractive_index_inside);
    }
    else
    {
        if (!refract(outgoing_dir,
                     Normal3F(0, 0, -1),
                     refractive_index_inside,
                     refractive_index_outside,
                     incident_dir))
        {
            return Spectrum(0.0f);
        }

        result = transmittance*(Spectrum(1.0f) - dielectric_reflector.evaluate(cosTheta(*incident_dir)));

        if (transport_mode == TransportMode::Radiance)
            result *= refractive_index_inside*refractive_index_inside/(refractive_index_outside*refractive_index_outside);
    }

    return result/absCosTheta(*incident_dir);
}

// SpecularBSDF method implementations

SpecularBSDF::SpecularBSDF(const ReflectanceSpectrum& reflectance,
                           const TransmittanceSpectrum& transmittance,
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

Spectrum SpecularBSDF::evaluate(const Vector3F& outgoing_dir,
                                const Vector3F& incident_dir) const
{
    return Spectrum(0.0f);
}

Spectrum SpecularBSDF::sample(const Vector3F& outgoing_dir,
                              Vector3F* incident_dir,
                              const Point2F& sample_point,
                              imp_float* pdf_value,
                              BXDFType* sampled_type = nullptr) const
{
    printSevereMessage("SpecularBSDF::sample is not implemented");
    return Spectrum(0.0f);
}

// LambertianBRDF method implementations

LambertianBRDF::LambertianBRDF(const ReflectanceSpectrum& reflectance)
    : BXDF::BXDF(BXDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
      reflectance(reflectance)
{}

Spectrum LambertianBRDF::evaluate(const Vector3F& outgoing_dir,
                                  const Vector3F& incident_dir) const
{
    return reflectance*IMP_ONE_OVER_PI;
}

Spectrum LambertianBRDF::sample(const Vector3F& outgoing_dir,
                                Vector3F* incident_dir,
                                const Point2F& sample_point,
                                imp_float* pdf_value,
                                BXDFType* sampled_type = nullptr) const
{
    *incident_dir = cosineWeightedHemisphereSample(sample_point);

    if (outgoing_dir.z < 0)
        incident_dir->z = -incident_dir->z;

    *pdf_value = pdf(outgoing_dir, *incident_dir);

    return evaluate(outgoing_dir, *incident_dir);
}

Spectrum LambertianBRDF::reduced(const Vector3F& outgoing_dir,
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

imp_float LambertianBRDF::pdf(const Vector3F& outgoing_dir,
                              const Vector3F& incident_dir) const
{
    return sameHemisphere(outgoing_dir, incident_dir)? absCosTheta(incident_dir)*IMP_ONE_OVER_PI : 0;
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
ReflectanceSpectrum fresnelReflectance(imp_float cos_incident_angle,
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

bool refract(const Vector3F& incident_dir,
             const Normal3F& incident_surface_normal,
             imp_float refractive_index_incident_medium,
             imp_float refractive_index_transmitted_medium,
             Vector3F* transmitted_dir)
{
    imp_float refractive_index_ratio = refractive_index_incident_medium/refractive_index_transmitted_medium;

    imp_float cos_incident_angle = incident_dir.dot(incident_surface_normal);
    imp_float sin_sq_incident_angle = std::max<imp_float>(0, 1 - cos_incident_angle*cos_incident_angle);
    imp_float sin_sq_transmitted_angle = refractive_index_ratio*refractive_index_ratio*sin_sq_incident_angle;

    // Total internal reflection
    if (sin_sq_transmitted_angle >= 1)
        return false;

    imp_float cos_transmitted_angle = std::sqrt(1 - sin_sq_transmitted_angle);

    *transmitted_dir = (refractive_index_ratio*cos_incident_angle - cos_transmitted_angle)*Vector3F(incident_surface_normal) -
                       refractive_index_ratio*incident_dir;

    return true;
}

} // RayImpact
} // Impact
