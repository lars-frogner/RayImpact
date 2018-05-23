#include "Model.hpp"

namespace Impact {
namespace RayImpact {

// GeometricModel method definitions

bool GeometricModel::intersect(const Ray& ray,
                               SurfaceScatteringEvent* scattering_event) const
{
    imp_float intersection_distance;

    if (!shape->intersect(ray, &intersection_distance, scattering_event))
        return false;

    // Return the intersection distance via the ray max_distance attribute
    ray.max_distance = intersection_distance;

    scattering_event->model = this;

    // Initialize scattering_event.medium_interface here

    return true;
}

// TransformedModel method definitions

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

} // RayImpact
} // Impact
