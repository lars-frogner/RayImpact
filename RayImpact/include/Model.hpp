#pragma once
#include "precision.hpp"
#include "BoundingBox.hpp"
#include "Ray.hpp"
#include "ScatteringEvent.hpp"

namespace Impact {
namespace RayImpact {

// Forward declarations
class AreaLight;
class Material;

// Model declarations

class Model {

public:
	
	virtual BoundingBoxF worldSpaceBoundingBox() const = 0;

	virtual bool intersect(const Ray& ray,
						   SurfaceScatteringEvent* scattering_event) const = 0;

	virtual bool hasIntersection(const Ray& ray) const = 0;

	virtual const AreaLight* getAreaLight() const = 0;

	virtual const Material* getMaterial() const = 0;

	virtual void computeScatteringFunctions(SurfaceScatteringEvent* scattering_event,
											//MemoryArena& arena, TransportMode mode,
											bool allow_multiple_lobes) const = 0;
};

} // RayImpact
} // Impact
