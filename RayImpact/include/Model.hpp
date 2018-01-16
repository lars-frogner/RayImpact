#pragma once
#include "precision.hpp"
#include "RegionAllocator.hpp"
#include "BoundingBox.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "AnimatedTransformation.hpp"
#include "ScatteringEvent.hpp"
#include "Shape.hpp"
#include <memory>

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
											RegionAllocator& allocator,
											//TransportMode mode,
											bool allow_multiple_lobes) const = 0;
};

// GeometricModel declarations

class GeometricModel : public Model {

private:

	std::shared_ptr<Shape> shape; // The shape associated with the model
	std::shared_ptr<Material> material; // The material associated with the model
	std::shared_ptr<AreaLight> area_light; // The area light (if any) associated with the model
	MediumInterface medium_interface; // The interface between the media on the inside and the outside of the model

public:

	GeometricModel(const std::shared_ptr<Shape>& shape,
				   const std::shared_ptr<Material>& material,
				   const std::shared_ptr<AreaLight>& area_light,
				   const MediumInterface& medium_interface);

	BoundingBoxF worldSpaceBoundingBox() const;
	
	bool intersect(const Ray& ray,
				   SurfaceScatteringEvent* scattering_event) const;
	
	bool hasIntersection(const Ray& ray) const;

	const AreaLight* getAreaLight() const;

	const Material* getMaterial() const;

	void computeScatteringFunctions(SurfaceScatteringEvent* scattering_event,
									RegionAllocator& allocator,
									//TransportMode mode,
									bool allow_multiple_lobes) const;
};

// TransformedModel declarations

class TransformedModel : public Model {

private:

	std::shared_ptr<Model> model; // The underlying model for the transformed model
	const AnimatedTransformation model_to_world; // The transformation from the shape's notion of world space to actual world space

public:

	TransformedModel(const std::shared_ptr<Model>& model,
				     const AnimatedTransformation& model_to_world);

	BoundingBoxF worldSpaceBoundingBox() const;
	
	bool intersect(const Ray& ray,
				   SurfaceScatteringEvent* scattering_event) const;
	
	bool hasIntersection(const Ray& ray) const;

	const AreaLight* getAreaLight() const;

	const Material* getMaterial() const;

	void computeScatteringFunctions(SurfaceScatteringEvent* scattering_event,
									RegionAllocator& allocator,
									//TransportMode mode,
									bool allow_multiple_lobes) const;
};

} // RayImpact
} // Impact
