#include "api.hpp"
#include "error.hpp"
#include "parallel.hpp"
#include "Matrix4x4.hpp"
#include "RegionAllocator.hpp"
#include "Transformation.hpp"
#include "AnimatedTransformation.hpp"
#include "Spectrum.hpp"
#include "ParameterSet.hpp"
#include "Shape.hpp"
#include "Model.hpp"
#include "Scene.hpp"
#include "Integrator.hpp"
#include <memory>
#include <map>
#include <vector>
#include <utility>

namespace Impact {
namespace RayImpact {

// API global variables

// Global rendering options
Options RIMP_OPTIONS;

// State tracking
enum class APIState {Uninitialized, Configuration, SceneDescription};
static APIState current_API_state = APIState::Uninitialized; // State of the Ray Impact API

// Current transformation tracking

constexpr uint32_t max_transformations = 2; // The number of transformations that can be stored in the current transformation set
constexpr uint32_t initial_transformation_bit = 1 << 0; // The bit indicating that the current transformation set has an initial transformation
constexpr uint32_t final_transformation_bit = 1 << 1; // The bit indicating that the current transformation set has a final transformation
constexpr uint32_t all_transformations_bits = (1 << max_transformations) - 1; // The bits indicating that the current transformation set has all transformation types

// Container for multiple transformations
class TransformationSet {

private:
	Transformation transformations[max_transformations];

public:

	Transformation& operator[](unsigned int idx)
	{
		return transformations[idx];
	}

	TransformationSet inverted() const
	{
		TransformationSet result;

		for (unsigned int idx = 0; idx < max_transformations; idx++)
			result.transformations[idx] = transformations[idx].inverted();

		return result;
	}

	bool isAnimated() const
	{
		for (unsigned int idx = 0; idx < max_transformations-1; idx++)
			if (transformations[idx] != transformations[idx+1])
				return true;

		return false;
	}
};

static uint32_t active_transformation_bits = all_transformation_bits; // Bit vector indicating the types of transformations stored in the current transformation set
static TransformationSet current_transformations; // Variable holding the currently active transformations

static std::map<std::string, TransformationSet> defined_coordinate_systems; // A table of named transformation sets corresponding to defined coordinate systems

// Configurations tracking

// Container for rendering configurations
struct Configurations
{
	imp_float transformation_start_time = 0.0f; // Point in time when the initial transformation applies
	imp_float transformation_end_time = 1.0f; // Point in time when the final transformation applies

	std::string accelerator_type = "default"; // Type of the acceleration structure to use
	ParameterSet accelerator_parameters; // Parameters for the acceleration structure

	std::string sampler_type = "stratified"; // Type of the sampler type to use
	ParameterSet sampler_parameters; // Parameters for the sampler

	std::string filter_type = "box"; // Type of the reconstruction filter type to use
	ParameterSet filter_parameters; // Parameters for the reconstruction filter

	std::string camera_type = "perspective"; // Type of the camera type to use
	ParameterSet camera_parameters; // Parameters for the camera

	std::string sensor_type = "default"; // Type of the sensor type to use
	ParameterSet sensor_parameters; // Parameters for the sensor

	std::string integrator_type = "default"; // Type of the integrator type to use
	ParameterSet integrator_parameters; // Parameters for the integrator

	TransformationSet camera_to_world; // The transformation set for the camera-to-world transformation

	std::map< std::string, std::shared_ptr<Medium> > defined_media; // A table of named media

	std::vector< std::shared_ptr<Model> > models; // List of models in the scene
	std::vector< std::shared_ptr<AreaLight> > lights; // List of area lights in the scene

	std::map< std::string, std::vector< std::shared_ptr<Model> > > objects; // Table of object instances in the scene
	std::vector< std::shared_ptr<Model> >* current_object = nullptr; // The current object instance

	Integrator* createIntegrator()
	{

	}

	Scene* createScene()
	{

	}
};

static std::unique_ptr<Configurations> configurations; // The current rendering configurations

// Scene content tracking

class GraphicsState {

public:

	std::string inside_medium; // The medium on the inside (if applicable)
	std::string outside_medium; // The medium on the outside

	std::string area_light_type = "";
	ParameterSet area_light_parameters;

	bool use_reverse_orientation = false;

	std::shared_ptr<Material> createMaterial(const ParameterSet& parameters)
	{

	}

	MediumInterface createMediumInterface(const ParameterSet& parameters)
	{

	}
};

// Container for stored transformations
class TransformationCache {

private:

	std::map< Transformation, std::pair<Transformation*, Transformation*> > cache; // Table of transformations and pointers to where they are stored
	RegionAllocator allocator; // Allocator for transformations

public:

	void lookup(const Transformation& transformation,
				Transformation** stored_transformation,
				Transformation** stored_inverse_transformation)
	{
		Transformation* transformation_ptr;
		Transformation* inverse_transformation_ptr;

		auto entry = cache.find(transformation);

		if (entry != cache.end())
		{
			// Get pointers to the stored transformation and its inverse
			transformation_ptr = entry->second.first;
			inverse_transformation_ptr = entry->second.second;
		}
		else
		{
			// Allocate memory for the transformation and its inverse
			transformation_ptr = allocator.allocate<Transformation>(2, false);
			inverse_transformation_ptr = transformation_ptr + 1;

			// Store the transformation and its inverse
			*transformation_ptr = transformation;
			*inverse_transformation_ptr = transformation.inverted();

			// Add the newly stored transformation to the cache
			cache[transformation] = std::make_pair(transformation_ptr, inverse_transformation_ptr);
		}
			
		// Set return pointers
		
		if (stored_transformation)
			*stored_transformation = transformation_ptr;
			
		if (stored_inverse_transformation)
			*stored_inverse_transformation = inverse_transformation_ptr;
	}
};

static GraphicsState current_graphics_state; // Holds the current graphics state

static std::vector<GraphicsState> graphics_state_stack; // Stack for graphics states
static std::vector<TransformationSet> transformation_stack; // Stack for transformation sets
static std::vector<uint32_t> active_transformation_bits_stack; // Stack for active transformation bit vectors

static TransformationCache transformation_cache; // Holds stored transformations so they can be reused multiple times

// API macros

#define verify_initialized(function_name) \
	if (current_API_state == APIState::Uninitialized) \
	{ \
		printErrorMessage("API system must be initialized before calling \"%s()\". Ignoring call.", function_name); \
		return; \
	} \
	else // required so that the trailing semicolon doesn't cause syntax error

#define verify_in_config_state(function_name) \
	if (current_API_state == APIState::Configuration) \
	{ \
		printErrorMessage("API system must be in the configuration state when calling \"%s()\". Ignoring call.", function_name); \
		return; \
	} \
	else // required so that the trailing semicolon doesn't cause syntax error

#define verify_in_scene_descript_state(function_name) \
	if (current_API_state == APIState::SceneDescription) \
	{ \
		printErrorMessage("API system must be in the scene generation state when calling \"%s()\". Ignoring call.", function_name); \
		return; \
	} \
	else // required so that the trailing semicolon doesn't cause syntax error

#define warn_if_transformation_is_animated(function_name) \
	do { if (current_transformations.isAnimated()) \
		     printWarningMessage("current transformation set is animated. Using initial transformation only in \"%s\".", function_name); \
	} while (false)  // required so that the trailing semicolon doesn't cause syntax error

#define for_active_transformations(loop_body) \
	for (uint32_t idx = 0; idx < max_transformations; idx++) \
		if (active_transformation_bits & (1 << idx)) { loop_body }

// Creation functions

std::vector< std::shared_ptr<Shape> > createShapes(const std::string& type,
												   const Transformation* model_to_world,
												   const Transformation* world_to_model,
												   bool use_reverse_orientation,
												   const ParameterSet& parameters)
{

}

std::shared_ptr<Model> CreateAccelerationStructure(const std::string& type,
												   const std::vector< std::shared_ptr<Model> >& models,
												   const ParameterSet& parameters)
{

}

std::shared_ptr<AreaLight> createAreaLight(const std::string& type,
										   const Transformation& light_to_world,
										   const MediumInterface& medium_interface,
										   const ParameterSet& parameters,
										   std::shared_ptr<Shape> shape)
{

}

// API function implementations

// Performs required initializations for the rendering system
void RIMP_Initialize(const Options& options)
{
	RIMP_OPTIONS = options;

	if (current_API_state != APIState::Uninitialized)
		printErrorMessage("API system has already been initialized");

	current_API_state = APIState::Configuration;

	configurations.reset(new Configurations);

	current_graphics_state = GraphicsState();

	initializeParallel(options.n_threads);

	SampledSpectrum::initialize();
}

// Performs cleanup for the rendering system
void RIMP_Cleanup()
{
	if (current_API_state == APIState::Uninitialized)
		printErrorMessage("API cleanup requested before initialization");
	else if (current_API_state == APIState::SceneDescription)
		printErrorMessage("API cleanup requested before finishing configuration");

	current_API_state = APIState::Uninitialized;

	configurations.reset(nullptr);

	cleanupParallel();
}

// Transformation functions

void RIMP_SetActiveTransformationsToAll()
{
	verify_initialized("SetActiveTransformationsToAll");
	active_transformation_bits = all_transformations_bits;
}

void RIMP_SetActiveTransformationsToInitial()
{
	verify_initialized("SetActiveTransformationsToInitial");
	active_transformation_bits = initial_transformation_bit;
}

void RIMP_SetActiveTransformationsToFinal()
{
	verify_initialized("SetActiveTransformationsToFinal");
	active_transformation_bits = final_transformation_bit;
}

void RIMP_UseIdentity()
{
	verify_initialized("UseIdentity");

	for_active_transformations(
		current_transformations[idx] = Transformation();
	)
}

void RIMP_UseTranslation(imp_float dx, imp_float dy, imp_float dz)
{
	verify_initialized("UseTranslation");

	for_active_transformations(
		current_transformations[idx] = Transformation::translation(Vector3F(dx, dy, dz));
	)
}

void RIMP_UseRotation(imp_float axis_x, imp_float axis_y, imp_float axis_z, imp_float angle)
{
	verify_initialized("UseRotation");

	for_active_transformations(
		current_transformations[idx] = Transformation::rotation(Vector3F(axis_x, axis_y, axis_z), angle);
	)
}

void RIMP_UseScaling(imp_float scale_x, imp_float scale_y, imp_float scale_z)
{
	verify_initialized("UseScaling");

	for_active_transformations(
		current_transformations[idx] = Transformation::scaling(scale_x, scale_y, scale_z);
	)
}

void RIMP_UseWorldToCamera(imp_float camera_pos_x, imp_float camera_pos_y, imp_float camera_pos_z,
						   imp_float     up_vec_x, imp_float     up_vec_y, imp_float     up_vec_z,
						   imp_float look_point_x, imp_float look_point_y, imp_float look_point_z)
{
	verify_initialized("UseWorldToCamera");

	for_active_transformations(
		current_transformations[idx] = Transformation::worldToCamera(Point3F(camera_pos_x, camera_pos_y, camera_pos_z),
																	 Vector3F(   up_vec_x,     up_vec_y,     up_vec_z),
																	 Point3F(look_point_x, look_point_y, look_point_z));
	)
}

void RIMP_UseTransformation(const imp_float matrix_elements[16])
{
	verify_initialized("UseTransformation");

	const Transformation& transformation = Transformation(Matrix4x4(matrix_elements));

	for_active_transformations(
		current_transformations[idx] = transformation;
	)
}

void RIMP_UseConcatenated(const imp_float matrix_elements[16])
{
	verify_initialized("UseConcatenated");

	const Transformation& transformation = Transformation(Matrix4x4(matrix_elements));

	for_active_transformations(
		current_transformations[idx] = current_transformations[idx]*transformation;
	)
}

void RIMP_DefineCoordinateSystem(const std::string& name)
{
	verify_initialized("DefineCoordinateSystem");

	defined_coordinate_systems[name] = current_transformations;
}

void RIMP_UseCoordinateSystem(const std::string& name)
{
	verify_initialized("UseCoordinateSystem");

	auto entry = defined_coordinate_systems.find(name);

	if (entry != defined_coordinate_systems.end())
		current_transformations = entry->second;
	else
		printWarningMessage("coordinate system \"%s\" not found. Ignoring call to \"UseCoordinateSystem()\".", name.c_str());
}

// Medium functions

void RIMP_DefineMedium(const std::string& name, const ParameterSet& parameters)
{
	verify_initialized("DefineMedium");

	//configurations->defined_media[name] = // medium;
}

void RIMP_UseMediumInterface(const std::string& inside_medium_name, const std::string& outside_medium_name)
{
	verify_initialized("UseMediumInterface");

	current_graphics_state.inside_medium = inside_medium_name;
	current_graphics_state.outside_medium = outside_medium_name;
}

// Configuration functions

void RIMP_SetTransformationTimes(imp_float start_time, imp_float end_time)
{
	verify_in_config_state("SetTransformationTimes");

	configurations->transformation_start_time = start_time;
	configurations->transformation_end_time = end_time;
}

void RIMP_SetAccelerationStructure(const std::string& type, const ParameterSet& parameters)
{
	verify_in_config_state("SetAccelerationStructure");

	configurations->accelerator_type = type;
	configurations->accelerator_parameters = parameters;
}

void RIMP_SetSampler(const std::string& type, const ParameterSet& parameters)
{
	verify_in_config_state("SetSampler");

	configurations->sampler_type = type;
	configurations->sampler_parameters = parameters;
}

void RIMP_SetFilter(const std::string& type, const ParameterSet& parameters)
{
	verify_in_config_state("SetFilter");

	configurations->filter_type = type;
	configurations->filter_parameters = parameters;
}

void RIMP_SetCamera(const std::string& type, const ParameterSet& parameters)
{
	verify_in_config_state("SetCamera");

	configurations->camera_type = type;
	configurations->camera_parameters = parameters;

	configurations->camera_to_world = current_transformations.inverted();
	defined_coordinate_systems["camera"] = configurations->camera_to_world;
}

void RIMP_SetCameraSensor(const std::string& type, const ParameterSet& parameters)
{
	verify_in_config_state("SetCameraSensor");

	configurations->sensor_type = type;
	configurations->sensor_parameters = parameters;
}

void RIMP_SetIntegrator(const std::string& type, const ParameterSet& parameters)
{
	verify_in_config_state("SetIntegrator");

	configurations->integrator_type = type;
	configurations->integrator_parameters = parameters;
}

// Scene description functions

void RIMP_BeginSceneDescription()
{
	verify_in_config_state("BeginSceneDescription");

	current_API_state = APIState::SceneDescription;

	for (unsigned int idx = 0; idx < max_transformations; idx++)
		current_transformations[idx] = Transformation();

	active_transformation_bits = all_transformations_bits;

	defined_coordinate_systems["world"] = current_transformations;
}

void RIMP_BeginAttribute()
{
	verify_in_scene_descript_state("BeginAttribute");

	graphics_state_stack.push_back(current_graphics_state);
	transformation_stack.push_back(current_transformations);
	active_transformation_bits_stack.push_back(active_transformation_bits);
}

void RIMP_EndAttribute()
{
	verify_in_scene_descript_state("EndAttribute");

	if (graphics_state_stack.empty())
	{
		printErrorMessage("unmatched \"EndAttribute()\" call encountered. Ignoring call.");
		return;
	}

	current_graphics_state = graphics_state_stack.back();
	current_transformations = transformation_stack.back();
	active_transformation_bits = active_transformation_bits_stack.back();

	graphics_state_stack.pop_back();
	transformation_stack.pop_back();
	active_transformation_bits_stack.pop_back();
}

void RIMP_BeginTransformation()
{
	verify_in_scene_descript_state("BeginTransformation");

	transformation_stack.push_back(current_transformations);
	active_transformation_bits_stack.push_back(active_transformation_bits);
}

void RIMP_EndTransformation()
{
	verify_in_scene_descript_state("EndTransformation");

	if (transformation_stack.empty())
	{
		printErrorMessage("unmatched \"EndTransformation()\" call encountered. Ignoring call.");
		return;
	}

	current_transformations = transformation_stack.back();
	active_transformation_bits = active_transformation_bits_stack.back();

	transformation_stack.pop_back();
	active_transformation_bits_stack.pop_back();
}

void RIMP_CreateModel(const std::string& type, const ParameterSet& parameters)
{
	verify_in_scene_descript_state("CreateModel");

	std::vector< std::shared_ptr<Model> > models;
	std::vector< std::shared_ptr<AreaLight> > area_lights;

	if (!current_transformations.isAnimated())
	{
		// Create shape(s) of the given type

		Transformation* model_to_world;
		Transformation* world_to_model;

		// Make sure the transformation is stored in the cache and retrieve its pointers
		transformation_cache.lookup(current_transformations[0], &model_to_world, &world_to_model);
		
		std::vector< std::shared_ptr<Shape> > shapes = createShapes(type,
																	model_to_world,
																	world_to_model,
																	current_graphics_state.use_reverse_orientation,
																	parameters);
		if (shapes.empty())
			return;

		// Create geometric model(s) for the shape(s)

		std::shared_ptr<Material> material = current_graphics_state.createMaterial(parameters);

		parameters.warnAboutUnusedParameters();

		MediumInterface medium_interface = current_graphics_state.createMediumInterface(parameters);

		for (auto shape : shapes)
		{
			std::shared_ptr<AreaLight> area_light;

			if (current_graphics_state.area_light_type != "")
			{
				area_light = createAreaLight(current_graphics_state.area_light_type,
											 current_transformations[0],
											 medium_interface,
											 current_graphics_state.area_light_parameters,
											 shape);

				area_lights.push_back(area_light);
			}

			models.push_back(std::make_shared<GeometricModel>(shape, material, area_light, medium_interface));
		}
	}
	else
	{
		if (current_graphics_state.area_light_type != "")
			printWarningMessage("area lights not supported for animated models. Ignoring area light.");

		// Create shape(s) of the given type

		// Only used identity transformation, as the actual (animated) transformation will be set in the model
		Transformation* identity;

		transformation_cache.lookup(Transformation(), &identity, nullptr);
		
		std::vector< std::shared_ptr<Shape> > shapes = createShapes(type,
																	identity,
																	identity,
																	current_graphics_state.use_reverse_orientation,
																	parameters);
		if (shapes.empty())
			return;

		// Create geometric model(s) for the shape(s)
		
		std::shared_ptr<Material> material = current_graphics_state.createMaterial(parameters);

		parameters.warnAboutUnusedParameters();

		MediumInterface medium_interface = current_graphics_state.createMediumInterface(parameters);

		for (auto shape : shapes)
			models.push_back(std::make_shared<GeometricModel>(shape, material, nullptr, medium_interface));

		// Create animated model for the geometric model(s)

		Transformation* model_to_world[2];
		
		transformation_cache.lookup(current_transformations[0], &model_to_world[0], nullptr);
		transformation_cache.lookup(current_transformations[1], &model_to_world[1], nullptr);

		AnimatedTransformation animated_model_to_world(model_to_world[0], model_to_world[1],
													   configurations->transformation_start_time,
													   configurations->transformation_end_time);

		// Create aggregate from the geometric models if there is more than one
		if (models.size() > 1)
		{
			//std::shared_ptr<Model> BVH = std::make_shared<BoundingVolumeHierarchy>(models);
			models.clear();
			//models.push_back(BVH);
		}

		models[0] = std::make_shared<TransformedModel>(models[0], animated_model_to_world);
	}

	// Store the model(s) in either the current object instance or the list of non-instanced models

	if (configurations->current_object)
	{
		if (!area_lights.empty())
			printWarningMessage("area lights not supported for instancing objects. Ignoring area light.");

		configurations->current_object->insert(configurations->current_object->end(), models.begin(), models.end());
	}
	else
	{
		configurations->models.insert(configurations->models.end(), models.begin(), models.end());
		
		if (!area_lights.empty())
			configurations->lights.insert(configurations->lights.end(), area_lights.begin(), area_lights.end());
	}
}

void RIMP_BeginObject(const std::string& name)
{
	verify_in_scene_descript_state("BeginObject");

	RIMP_BeginAttribute();

	if (configurations->current_object)
		printErrorMessage("\"BeginObject()\" called from inside object definition");

	configurations->objects[name] = std::vector< std::shared_ptr<Model> >();

	configurations->current_object = &(configurations->objects[name]);
}

void RIMP_EndObject()
{
	verify_in_scene_descript_state("EndObject");

	if (!configurations->current_object)
		printErrorMessage("\"EndObject()\" called from outside object definition");

	configurations->current_object = nullptr;

	RIMP_EndAttribute();
}

void RIMP_CreateObjectInstance(const std::string& name)
{
	verify_in_scene_descript_state("CreateObjectInstance");

	if (configurations->current_object)
	{
		printErrorMessage("\"CreateObjectInstance()\" called from inside object definition. Ignoring call.");
		return;
	}

	auto entry = configurations->objects.find(name);

	if (entry == configurations->objects.end())
	{
		printWarningMessage("object \"%s\" not found. Ignoring call to \"CreateObjectInstance()\".", name.c_str());
		return;
	}

	std::vector< std::shared_ptr<Model> >& object_models = entry->second;

	if (object_models.empty())
		return;
	
	// Create aggregate from the models in the object if there is more than one
	if (object_models.size() > 1)
	{
		std::shared_ptr<Model> aggregate(CreateAccelerationStructure(configurations->accelerator_type,
																	 object_models,
																	 configurations->accelerator_parameters));
		//if (!aggregate)
		//	aggregate = std::make_shared<BoundingVolumeHierarchy>(object_models);

		object_models.clear();
		object_models.push_back(aggregate);
	}
	
	Transformation* object_to_world[2];
		
	transformation_cache.lookup(current_transformations[0], &object_to_world[0], nullptr);
	transformation_cache.lookup(current_transformations[1], &object_to_world[1], nullptr);

	AnimatedTransformation animated_object_to_world(object_to_world[0], object_to_world[1],
												    configurations->transformation_start_time,
												    configurations->transformation_end_time);

	std::shared_ptr<Model> model(std::make_shared<TransformedModel>(object_models[0], animated_object_to_world));

	configurations->models.push_back(model);
}

void RIMP_EndSceneDescription()
{
	verify_in_scene_descript_state("EndSceneDescription");

	while (!graphics_state_stack.empty())
	{
		printWarningMessage("missing \"EndAttribute()\" call");
		graphics_state_stack.pop_back();
		transformation_stack.pop_back();
		active_transformation_bits_stack.pop_back();
	}

	while (!transformation_stack.empty())
	{
		printWarningMessage("missing \"EndTransformation()\" call");
		transformation_stack.pop_back();
		active_transformation_bits_stack.pop_back();
	}

	std::unique_ptr<Integrator> integrator(configurations->createIntegrator());
	std::unique_ptr<Scene> scene(configurations->createScene());

	if (integrator && scene)
		integrator->render(*scene);

	current_API_state = APIState::Configuration;

	for (unsigned int idx = 0; idx < max_transformations; idx++)
		current_transformations[idx] = Transformation();

	active_transformation_bits = all_transformations_bits;

	defined_coordinate_systems.erase(defined_coordinate_systems.begin(), defined_coordinate_systems.end());
}

} // RayImpact
} // Impact
