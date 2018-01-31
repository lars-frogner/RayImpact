#include "Scene.hpp"

namespace Impact {
namespace RayImpact {

// Scene method implementations

Scene::Scene(std::shared_ptr<Model> model_aggregate,
             const std::vector< std::shared_ptr<Light> >& lights)
    : model_aggregate(model_aggregate),
      lights(lights),
      world_bounding_box(model_aggregate->worldSpaceBoundingBox())
{
    for (const auto& light : lights)
    {
        light->preprocess(*this);
    }
}

const BoundingBoxF& Scene::worldSpaceBoundingBox() const
{
    return world_bounding_box;
}

bool Scene::intersect(const Ray& ray, SurfaceScatteringEvent* scattering_event) const
{
    return model_aggregate->intersect(ray, scattering_event);
}

bool Scene::hasIntersection(const Ray& ray) const
{
    return model_aggregate->hasIntersection(ray);
}

} // RayImpact
} // Impact
