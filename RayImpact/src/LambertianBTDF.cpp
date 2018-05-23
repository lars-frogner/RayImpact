#include "LambertianBTDF.hpp"
#include "sampling.hpp"

namespace Impact {
namespace RayImpact {

// LambertianBTDF method definitions

Spectrum LambertianBTDF::sample(const Vector3F& outgoing_direction,
                                Vector3F* incident_direction,
                                const Point2F& uniform_sample,
                                imp_float* pdf_value,
                                BXDFType* sampled_type /* = nullptr */) const
{
    *incident_direction = cosineWeightedHemisphereSample(uniform_sample);

    if (outgoing_direction.z > 0)
        incident_direction->z = -incident_direction->z;

    *pdf_value = pdf(outgoing_direction, *incident_direction);

    return evaluate(outgoing_direction, *incident_direction);
}

} // RayImpact
} // Impact
