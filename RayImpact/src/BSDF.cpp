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
                      const Point2F& uniform_sample,
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
                            const Point2F& uniform_sample,
                            imp_float* pdf_value,
                            BXDFType* sampled_type /* = nullptr */) const
{
    return scale*bxdf->sample(outgoing_direction,
                              incident_direction,
                              uniform_sample,
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

Spectrum BSDF::sample(const Vector3F& outgoing_direction,
                      Vector3F* incident_direction,
                      const Point2F& uniform_sample,
                      imp_float* pdf_value,
                      BXDFType type /* = BSDF_ALL */) const
{
    printSevereMessage("BSDF::sample was called without being implemented");
    return Spectrum(0.0f);
}

} // RayImpact
} // Impact
