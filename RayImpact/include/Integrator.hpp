#pragma once
#include "RegionAllocator.hpp"
#include "Scene.hpp"
#include "Sampler.hpp"
#include "Camera.hpp"
#include "Spectrum.hpp"
#include "Ray.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// Integrator declarations

class Integrator {

public:

	virtual void render(const Scene& scene) = 0;
};

// SampleIntegrator declarations

class SampleIntegrator : public Integrator {

private:

	std::shared_ptr<Sampler> sampler; // The sample generator used by the integrator

protected:

	std::shared_ptr<const Camera> camera; // The camera providing eye rays and holding the sensor with the final image

public:

	SampleIntegrator(std::shared_ptr<const Camera> camera,
					 std::shared_ptr<Sampler> sampler);

	virtual void preprocess(const Scene& scene, Sampler& sampler);

	virtual RadianceSpectrum incidentRadiance(const RayWithOffsets& outgoing_ray,
											  const Scene& scene,
											  Sampler& sampler,
											  RegionAllocator& allocator,
											  unsigned int scattering_count = 0) const = 0;

	void render(const Scene& scene);
};

} // RayImpact
} // Impact
