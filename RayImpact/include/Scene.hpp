#pragma once
#include "BoundingBox.hpp"
#include "Ray.hpp"
#include "ScatteringEvent.hpp"
#include "Model.hpp"
#include "Light.hpp"
#include <memory>
#include <vector>

namespace Impact {
namespace RayImpact {

// Scene declarations

class Scene {

private:

    std::shared_ptr<Model> model_aggregate; // An aggregate of all models in the scene
    BoundingBoxF world_bounding_box; // Bounding box encompassing all models in the scene

public:

    std::vector< std::shared_ptr<Light> > lights; // All lights in the scene

    Scene(std::shared_ptr<Model> model_aggregate,
          const std::vector< std::shared_ptr<Light> >& lights);

    const BoundingBoxF& worldSpaceBoundingBox() const;

    bool intersect(const Ray& ray, SurfaceScatteringEvent* scattering_event) const;

    bool hasIntersection(const Ray& ray) const;
};

// Scene inline method definitions

inline const BoundingBoxF& Scene::worldSpaceBoundingBox() const
{
    return world_bounding_box;
}

inline bool Scene::intersect(const Ray& ray, SurfaceScatteringEvent* scattering_event) const
{
    return model_aggregate->intersect(ray, scattering_event);
}

inline bool Scene::hasIntersection(const Ray& ray) const
{
    return model_aggregate->hasIntersection(ray);
}

} // RayImpact
} // Impact
