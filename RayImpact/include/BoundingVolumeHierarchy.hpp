#pragma once
#include "Model.hpp"
#include "RegionAllocator.hpp"
#include "ParameterSet.hpp"
#include <memory>
#include <vector>

namespace Impact {
namespace RayImpact {

// BVHModelInfo implementation

class BVHModelBound {

public:

    size_t model_idx;
    BoundingBoxF bounding_box;
    Point3F centroid;

    BVHModelBound(size_t model_idx, const BoundingBoxF& model_bounding_box)
        : model_idx(model_idx),
          bounding_box(model_bounding_box),
          centroid(0.5f*(model_bounding_box.lower_corner + model_bounding_box.upper_corner))
    {}
};

// BVHNode implementation

class BVHNode {

public:

    BoundingBoxF bounding_box;
    BVHNode* child_nodes[2];
    unsigned int split_axis;
    unsigned int first_model_idx;
    unsigned int n_models;

    void initializeAsLeafNode(unsigned int first_idx, unsigned int n, const BoundingBoxF& bounds)
    {
        first_model_idx = first_idx;
        n_models = n;
        bounding_box = bounds;
        child_nodes[0] = nullptr;
        child_nodes[1] = nullptr;
    }

    void initializeAsInteriorNode(unsigned int axis, BVHNode* first_child, BVHNode* second_child)
    {
        split_axis = axis;
        n_models = 0;
        bounding_box = unionOf(first_child->bounding_box, second_child->bounding_box);
        child_nodes[0] = first_child;
        child_nodes[1] = second_child;
    }
};

// BoundingVolumeHierarchy declarations

class BoundingVolumeHierarchy : public AccelerationStructure {

public:
	enum class SplitMethod { SAH, HLBVH, MIDDLE, EQUAL_COUNTS };

private:

    const unsigned int max_models_in_node; // Maxium allowed number of models that can be contained in a BVH node
    const SplitMethod split_method; // The method to use for partitioning models
    std::vector< std::shared_ptr<Model> > models; // All the models contained in the BVH

    BVHNode* buildRecursive(RegionAllocator& allocator,
                            std::vector<BVHModelBound>& model_bounds,
                            unsigned int start_model_idx,
                            unsigned int end_model_idx,
                            unsigned int* n_models_total,
                            std::vector< std::shared_ptr<Model> >& models_ordered);

public:

    BoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& contained_models,
                            unsigned int max_models_in_node = 1,
                            SplitMethod split_method = SplitMethod::SAH);

    BoundingBoxF worldSpaceBoundingBox() const;

    bool intersect(const Ray& ray,
                   SurfaceScatteringEvent* scattering_event) const;

    bool hasIntersection(const Ray& ray) const;
};

// BoundingVolumeHierarchy function declarations

std::shared_ptr<Model> createBoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& models,
                                                     const ParameterSet& parameters);

} // RayImpact
} // Impact
