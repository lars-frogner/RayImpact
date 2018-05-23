#include "SpecularBTDF.hpp"

namespace Impact {
namespace RayImpact {

// SpecularBTDF method definitions

Spectrum SpecularBTDF::sample(const Vector3F& outgoing_direction,
                              Vector3F* incident_direction,
                              const Point2F& uniform_sample,
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

        result = transmittance*(Spectrum(1.0f) - 
								dielectric_reflector.evaluate(cosTheta(*incident_direction)));

        if (transport_mode == TransportMode::Radiance)
            result *= refractive_index_outside*refractive_index_outside/
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

        result = transmittance*(Spectrum(1.0f) - 
								dielectric_reflector.evaluate(cosTheta(*incident_direction)));

        if (transport_mode == TransportMode::Radiance)
            result *= refractive_index_inside*refractive_index_inside/
					  (refractive_index_outside*refractive_index_outside);
    }

    return result/absCosTheta(*incident_direction);
}

} // RayImpact
} // Impact
