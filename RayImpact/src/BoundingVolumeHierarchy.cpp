#include "BoundingVolumeHierarchy.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// BoundingVolumeHierarchy method implementations

BoundingVolumeHierarchy::BoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& models,
                                                 unsigned int max_models_in_node /* = 1 */,
                                                 SplitMethod split_method /* = SplitMethod::SAH */)
    : max_models_in_node(std::min(255u, max_models_in_node)),
      split_method(split_method),
      models(models)
{}

BoundingBoxF BoundingVolumeHierarchy::worldSpaceBoundingBox() const
{

}

bool BoundingVolumeHierarchy::intersect(const Ray& ray,
                                        SurfaceScatteringEvent* scattering_event) const
{

}

bool BoundingVolumeHierarchy::hasIntersection(const Ray& ray) const
{

}

// BoundingVolumeHierarchy creation

std::shared_ptr<Model> createBoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& models,
                                                     const ParameterSet& parameters)
{
    unsigned int max_models_in_node = (unsigned int)std::abs(parameters.getSingleIntValue("max_models_in_node", 1));
    const std::string& split_method_name = parameters.getSingleStringValue("split_method", "sah");

    BoundingVolumeHierarchy::SplitMethod split_method = BoundingVolumeHierarchy::SplitMethod::SAH;

    if (split_method_name == "hlbvh")
        split_method = BoundingVolumeHierarchy::SplitMethod::HLBVH;
    else if (split_method_name == "middle")
        split_method = BoundingVolumeHierarchy::SplitMethod::MIDDLE;
    else if (split_method_name == "equal_counts")
        split_method = BoundingVolumeHierarchy::SplitMethod::EQUAL_COUNTS;
    else if (split_method_name != "sah")
    {
        printErrorMessage("split method \"%s\" for bounding volume hierarchy is invalid. Using SAH.", split_method_name.c_str());
        split_method = BoundingVolumeHierarchy::SplitMethod::SAH;
    }

    return std::make_shared<BoundingVolumeHierarchy>(models,
                                                     max_models_in_node,
                                                     split_method);
}

} // RayImpact
} // Impact
