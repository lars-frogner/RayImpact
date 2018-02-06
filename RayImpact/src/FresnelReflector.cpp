#include "FresnelReflector.hpp"

namespace Impact {
namespace RayImpact {

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

} // RayImpact
} // Impact
