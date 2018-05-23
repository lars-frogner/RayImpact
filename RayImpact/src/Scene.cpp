#include "Scene.hpp"

namespace Impact {
namespace RayImpact {

// Scene method definitions

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

} // RayImpact
} // Impact
