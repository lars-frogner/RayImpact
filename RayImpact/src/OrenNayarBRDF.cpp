#include "OrenNayarBRDF.hpp"

namespace Impact {
namespace RayImpact {

// OrenNayarBRDF method implementations

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

} // RayImpact
} // Impact
