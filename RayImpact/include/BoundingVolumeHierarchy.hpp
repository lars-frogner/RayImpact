#pragma once
#include "Model.hpp"
#include "ParameterSet.hpp"
#include <memory>
#include <vector>

namespace Impact {
namespace RayImpact {

// BoundingVolumeHierarchy declarations

class BoundingVolumeHierarchy : public AccelerationStructure {

private:

    enum class SplitMethod;

    const unsigned int max_models_in_node; // Maxium allowed number of models that can be contained in a BVH node
    const SplitMethod split_method; // The method to use for partitioning models
    std::vector< std::shared_ptr<Model> > models; // All the models contained in the BVH

public:

    enum class SplitMethod { SAH, HLBVH, MIDDLE, EQUAL_COUNTS };

    BoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& models,
                            unsigned int max_models_in_node = 1,
                            SplitMethod split_method = SplitMethod::SAH);

    BoundingBoxF worldSpaceBoundingBox() const;

    bool intersect(const Ray& ray,
                   SurfaceScatteringEvent* scattering_event) const;

    bool hasIntersection(const Ray& ray) const;
};

// BoundingVolumeHierarchy creation

std::shared_ptr<Model> createBoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& models,
                                                     const ParameterSet& parameters);

} // RayImpact
} // Impact
