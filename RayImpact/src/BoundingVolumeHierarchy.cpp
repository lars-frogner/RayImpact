#include "BoundingVolumeHierarchy.hpp"
#include "error.hpp"
#include "api.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// BoundingVolumeHierarchy method definitions

BoundingVolumeHierarchy::BoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& contained_models,
                                                 unsigned int max_models_in_node /* = 1 */,
                                                 SplitMethod split_method /* = SplitMethod::SAH */)
    : max_models_in_node(std::min(255u, max_models_in_node)),
      split_method(split_method),
      models(contained_models)
{
	if (models.size() == 0)
        return;

    /*
    std::vector<BVHModelBound> model_bounds;
    model_bounds.reserve(models.size());

    // Create list of bounding information for the models
    for (size_t i = 0; i < models.size(); i++)
        model_bounds.emplace_back(i, models[i]->worldSpaceBoundingBox());

    RegionAllocator allocator(1024*1024);
    unsigned int n_nodes_total = 0;
    std::vector< std::shared_ptr<Model> > models_ordered;
    BVHNode* root_node;

    // Build the BVH using the given split method
    if (split_method == SplitMethod::HLBVH)
    {
        //root_node = buildHLBVH(allocator, model_bounds, &n_nodes_total, models_ordered);
    }
    else
    {
        root_node = buildRecursive(allocator, model_bounds, 0, models.size(), &n_nodes_total, models_ordered);
    }

    models.swap(models_ordered);
	*/
}

/*
BVHNode* BoundingVolumeHierarchy::buildRecursive(RegionAllocator& allocator,
                                                 std::vector<BVHModelBound>& model_bounds,
                                                 unsigned int start_model_idx,
                                                 unsigned int end_model_idx,
                                                 unsigned int* n_models_total,
                                                 std::vector< std::shared_ptr<Model> >& models_ordered)
{
    // Allocate memory for new node
    BVHNode* node = allocator.allocate<BVHNode>();
    (*n_models_total)++;

    BoundingBoxF bounding_box;

    // Find the bounding box encompassing all models in the given subset
    for (unsigned int i = start_model_idx; i < end_model_idx; i++)
        bounding_box = unionOf(bounding_box, model_bounds[i].bounding_box);

    unsigned int n_models = end_model_idx - start_model_idx;

    if (n_models == 1)
    {
        // If there is only one model, create leaf node

        unsigned int first_model_idx = (unsigned int)models_ordered.size();
        
        models_ordered.push_back(models[model_bounds[start_model_idx].model_idx]);

        node->initializeAsLeafNode(first_model_idx, n_models, bounding_box);

        return node;
    }
    else
    {
        BoundingBoxF centroid_bounding_box;
        
        // Find the bounding box encompassing all bounding box centroids in the given subset
        for (unsigned int i = start_model_idx; i < end_model_idx; i++)
            centroid_bounding_box = unionOf(centroid_bounding_box, model_bounds[i].centroid);

        // Partition the subset along the dimension with the largest span of centroids
        unsigned int partition_dimension = centroid_bounding_box.maxDimension();

        unsigned int middle_model_idx;

        if (centroid_bounding_box.upper_corner[partition_dimension] == centroid_bounding_box.lower_corner[partition_dimension])
        {
            // If all centroids have the same position along the partition dimension, we just put the models in a leaf node

            unsigned int first_model_idx = (unsigned int)models_ordered.size();
        
            for (unsigned int i = start_model_idx; i < end_model_idx; i++)
                models_ordered.push_back(models[model_bounds[i].model_idx]);

            node->initializeAsLeafNode(first_model_idx, n_models, bounding_box);

            return node;
        }
        else
        {
            // Use the givens split method to partition the subset into two smaller subsets along the partition dimension
            switch (split_method)
            {
                case (SplitMethod::MIDDLE):

                    // Find middle of partition range
                    imp_float middle_coord = 0.5f*(centroid_bounding_box.lower_corner[partition_dimension] +
                                                   centroid_bounding_box.upper_corner[partition_dimension]);

                    // Partition the models into two subsets, one on each side of the middle of the partition range
                    BVHModelBound* middle_bound = std::partition(&(model_bounds[start_model_idx]),
                                                                 &(model_bounds[end_model_idx-1])+1,
                                                                 [partition_dimension, middle_coord](const BVHModelBound& model_bound)
                                                                 {
                                                                     return model_bound.centroid[partition_dimension] < middle_coord;
                                                                 });

                    // Find index of the first model in the upper subset
                    middle_model_idx = middle_bound - &(model_bounds[0]);

                    // If one of the two subsets is empty, try another next partitioning method instead
                    if (middle_model_idx != start_model_idx && middle_model_idx != end_model_idx)
                        break;

                case (SplitMethod::EQUAL_COUNTS):

                    middle_model_idx = (start_model_idx + end_model_idx)/2;

                    // Partition the models into two subsets of the same size, with the smallest coordinates in one and the largest in the other
                    std::nth_element(&(model_bounds[start_model_idx]),
                                     &(model_bounds[middle_model_idx]),
                                     &(model_bounds[end_model_idx-1])+1,
                                     [partition_dimension](const BVHModelBound& model_bound_1, const BVHModelBound& model_bound_2)
                                     {
                                         return model_bound_1.centroid[partition_dimension] < model_bound_2.centroid[partition_dimension];
                                     });

                    break;

                case (SplitMethod::SAH):

                    if (n_models <= 4)
                    {
                        middle_model_idx = (start_model_idx + end_model_idx)/2;

                        // Partition the models into two subsets of the same size, with the smallest coordinates in one and the largest in the other
                        std::nth_element(&(model_bounds[start_model_idx]),
                                         &(model_bounds[middle_model_idx]),
                                         &(model_bounds[end_model_idx-1])+1,
                                         [partition_dimension](const BVHModelBound& model_bound_1, const BVHModelBound& model_bound_2)
                                         {
                                             return model_bound_1.centroid[partition_dimension] < model_bound_2.centroid[partition_dimension];
                                         });
                    }
                    else
                    {
                        printSevereMessage("SAH partitioning not implemented for BVH");
                    }

                    break;

                default:
                    printSevereMessage("tried to build BVH with unimplemented split method");
            }

            // Create interior node pointing to the nodes of the two partitioned subsets
            node->initializeAsInteriorNode(partition_dimension,
                                           buildRecursive(allocator,
                                                          model_bounds,
                                                          start_model_idx,
                                                          middle_model_idx,
                                                          n_models_total,
                                                          models_ordered),
                                           buildRecursive(allocator,
                                                          model_bounds,
                                                          middle_model_idx,
                                                          end_model_idx,
                                                          n_models_total,
                                                          models_ordered));
        }
    }

    return node;
}
*/

BoundingBoxF BoundingVolumeHierarchy::worldSpaceBoundingBox() const
{
	BoundingBoxF bounding_box;

	for (auto iter = models.begin(); iter != models.end(); iter++)
		bounding_box = unionOf(bounding_box, (*iter)->worldSpaceBoundingBox());

	return bounding_box;
}

bool BoundingVolumeHierarchy::intersect(const Ray& ray,
                                        SurfaceScatteringEvent* scattering_event) const
{
	bool has_intersection = false;
	imp_float closest_distance = IMP_INFINITY;

	for (auto iter = models.begin(); iter != models.end(); iter++)
	{
		SurfaceScatteringEvent tmp_scattering_event;
		Ray tmp_ray(ray);
		if ((*iter)->intersect(tmp_ray, &tmp_scattering_event) && tmp_ray.max_distance < closest_distance)
		{
			has_intersection = true;
			closest_distance = tmp_ray.max_distance;
			*scattering_event = tmp_scattering_event;
		}
	}

	return has_intersection;
}

bool BoundingVolumeHierarchy::hasIntersection(const Ray& ray) const
{
	for (auto iter = models.begin(); iter != models.end(); iter++)
	{
		Ray tmp_ray(ray);
		if ((*iter)->hasIntersection(tmp_ray))
			return true;
	}

	return false;
}

// BoundingVolumeHierarchy function definitions

std::shared_ptr<Model> createBoundingVolumeHierarchy(const std::vector< std::shared_ptr<Model> >& models,
                                                     const ParameterSet& parameters)
{
    unsigned int max_node_size = (unsigned int)std::abs(parameters.getSingleIntValue("max_node_size", 1));
    std::string split_method_name = parameters.getSingleStringValue("split_method", "sah");

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
	
	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Acceleration structure:"
						 "\n    %-20s%s"
						 "\n    %-20s%u"
						 "\n    %-20s%u"
						 "\n    %-20s%s",
						 "Type:", "Bounding volume hierarchy",
						 "Contained models:", models.size(),
						 "Max node size:", max_node_size,
						 "Split method:", split_method_name.c_str());
	}

    return std::make_shared<BoundingVolumeHierarchy>(models,
                                                     max_node_size,
                                                     split_method);
}

} // RayImpact
} // Impact
