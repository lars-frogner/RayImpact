#pragma once
#include "Integrator.hpp"
#include "ParameterSet.hpp"

namespace Impact {
namespace RayImpact {

// WhittedIntegrator declarations

class WhittedIntegrator : public SampleIntegrator {

private:
    const unsigned int max_scattering_count; // Maxium number of allowed scatterings for each eye ray

public:

    WhittedIntegrator(std::shared_ptr<const Camera> camera,
                      std::shared_ptr<Sampler> sampler,
                      unsigned int max_scattering_count);

    RadianceSpectrum incidentRadiance(const RayWithOffsets& outgoing_ray,
                                      const Scene& scene,
                                      Sampler& sampler,
                                      RegionAllocator& allocator,
                                      unsigned int scattering_count = 0) const;
};

// WhittedIntegrator creation

Integrator* createWhittedIntegrator(std::shared_ptr<const Camera> camera,
                                    std::shared_ptr<Sampler> sampler,
                                    const ParameterSet& parameters);

} // RayImpact
} // Impact
