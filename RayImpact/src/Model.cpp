#include "Model.hpp"
#include "error.hpp"

namespace Impact {
namespace RayImpact {

// GeometricModel method implementations

GeometricModel::GeometricModel(const std::shared_ptr<Shape>& shape,
							   const std::shared_ptr<Material>& material,
							   const std::shared_ptr<AreaLight>& area_light,
							   const MediumInterface& medium_interface)
	: shape(shape),
	  material(material),
	  area_light(area_light),
	  medium_interface(medium_interface)
{}

BoundingBoxF GeometricModel::worldSpaceBoundingBox() const
{
	return shape->worldSpaceBoundingBox();
}
	
bool GeometricModel::intersect(const Ray& ray,
							   SurfaceScatteringEvent* scattering_event) const
{
	imp_float intersection_distance;

	if (!shape->intersect(ray, &intersection_distance, scattering_event))
		return false;

	// Return the intersection distance via the ray max_distance attribute
	ray.max_distance = intersection_distance;

	scattering_event->model = this;

	// Initialize scattering_event->medium_interface here

	return true;
}
	
bool GeometricModel::hasIntersection(const Ray& ray) const
{
	return shape->hasIntersection(ray);
}

const AreaLight* GeometricModel::getAreaLight() const
{
	return area_light.get();
}

const Material* GeometricModel::getMaterial() const
{
	return material.get();
}

void GeometricModel::computeScatteringFunctions(SurfaceScatteringEvent* scattering_event,
												RegionAllocator& allocator,
												//TransportMode mode,
												bool allow_multiple_lobes) const
{
	if (material)
	{
		/*material->computeScatteringFunctions(scattering_event,
											 allocator,
											 //mode,
											 allow_multiple_lobes);*/
	}
}

// TransformedModel method implementations

TransformedModel::TransformedModel(const std::shared_ptr<Model>& model,
								   const AnimatedTransformation& model_to_world)
	: model(model),
	  model_to_world(model_to_world)
{}

BoundingBoxF TransformedModel::worldSpaceBoundingBox() const
{
	return model_to_world.encompassMotionInBoundingBox(model->worldSpaceBoundingBox());
}
	
bool TransformedModel::intersect(const Ray& ray,
							     SurfaceScatteringEvent* scattering_event) const
{
	// Find transformation matrix at the time of the ray
	Transformation interpolated_model_to_world;
	model_to_world.computeInterpolatedTransformation(&interpolated_model_to_world, ray.time);
	
	// Transform ray from world space to model space
	const Ray& model_space_ray = interpolated_model_to_world.inverted()(ray);

	// Perform intersection
	if (!model->intersect(model_space_ray, scattering_event))
		return false;

	// Return the intersection distance via the ray max_distance attribute
	ray.max_distance = model_space_ray.max_distance;
	
	// Transform scattering event from model space to world space
	*scattering_event = interpolated_model_to_world(*scattering_event);

	return true;
}
	
bool TransformedModel::hasIntersection(const Ray& ray) const
{
	Transformation interpolated_model_to_world;
	model_to_world.computeInterpolatedTransformation(&interpolated_model_to_world, ray.time);

	return model->hasIntersection(interpolated_model_to_world.inverted()(ray));
}

const AreaLight* TransformedModel::getAreaLight() const
{
	printSevereMessage("\"getAreaLight()\" method of TransformedModel was called");
	return nullptr;
}

const Material* TransformedModel::getMaterial() const
{
	printSevereMessage("\"getMaterial()\" method of TransformedModel was called");
	return nullptr;
}

void TransformedModel::computeScatteringFunctions(SurfaceScatteringEvent* scattering_event,
												RegionAllocator& allocator,
												//TransportMode mode,
												bool allow_multiple_lobes) const
{
	printSevereMessage("\"computeScatteringFunctions()\" method of TransformedModel was called");
}

} // RayImpact
} // Impact
