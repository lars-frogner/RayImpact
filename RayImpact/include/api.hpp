#pragma once
#include "geometry.hpp"
#include "ParameterSet.hpp"
#include <string>

namespace Impact {
namespace RayImpact {

struct Options
{
    unsigned int n_threads = 0; // The number of threads to use for parallelization (determined automatically if set to 0)
    std::string image_filename = "out.pfm"; // The filename to use for the rendered image
	int verbosity = 0;
};

extern Options RIMP_OPTIONS; // Global rendering options

// API macros

#define IMP_MIN_VERBOSITY 0
#define IMP_MAX_VERBOSITY 3

#define IMP_CORE_VERBOSITY 1
#define IMP_SHAPES_VERBOSITY 2
#define IMP_LIGHTS_VERBOSITY 2
#define IMP_MATERIALS_VERBOSITY 2
#define IMP_CALLS_VERBOSITY 3

// API function declarations

void parseSceneDescriptionFiles(const std::vector<std::string>& filenames);

void RIMP_SetOption(const std::string& option, const std::string& value);

bool needsCleanup();

void RIMP_Initialize();

void RIMP_Cleanup();

void RIMP_SetActiveTransformationsToAll();

void RIMP_SetActiveTransformationsToInitial();

void RIMP_SetActiveTransformationsToFinal();

void RIMP_UseIdentity();

void RIMP_UseTranslation(const Vector3F& displacement);

void RIMP_ApplyTranslation(const Vector3F& displacement);

void RIMP_UseRotation(const Vector3F& axis, imp_float angle);

void RIMP_ApplyRotation(const Vector3F& axis, imp_float angle);

void RIMP_UseScaling(const Vector3F& scaling);

void RIMP_ApplyScaling(const Vector3F& scaling);

void RIMP_UseWorldToCamera(const Point3F& camera_position,
                           const Vector3F& up_vector,
                           const Point3F& look_point);

void RIMP_ApplyWorldToCamera(const Point3F& camera_position,
                             const Vector3F& up_vector,
                             const Point3F& look_point);

void RIMP_UseTransformation(const imp_float matrix_elements[16]);

void RIMP_ApplyTransformation(const imp_float matrix_elements[16]);

void RIMP_DefineCoordinateSystem(const std::string& name);

void RIMP_DefineMedium(const std::string& name, const ParameterSet& parameters);

void RIMP_UseMediumInterface(const std::string& inside_medium_name, const std::string& outside_medium_name);

void RIMP_SetTransformationTimes(imp_float start_time, imp_float end_time);

void RIMP_SetAccelerationStructure(const std::string& type, const ParameterSet& parameters);

void RIMP_SetSampler(const std::string& type, const ParameterSet& parameters);

void RIMP_SetFilter(const std::string& type, const ParameterSet& parameters);

void RIMP_SetCamera(const std::string& type, const ParameterSet& parameters);

void RIMP_SetCameraSensor(const ParameterSet& parameters);

void RIMP_SetIntegrator(const std::string& type, const ParameterSet& parameters);

void RIMP_BeginSceneDescription();

void RIMP_BeginAttribute();

void RIMP_EndAttribute();

void RIMP_BeginTransformation();

void RIMP_EndTransformation();

void RIMP_DefineTexture(const std::string& name,
                        const std::string& texture_data_type,
                        const std::string& texture_type,
                        const ParameterSet& parameters);

void RIMP_UseMaterial(const std::string& type, const ParameterSet& parameters);

void RIMP_DefineMaterial(const std::string& name, const std::string& type, const ParameterSet& parameters);

void RIMP_UseDefinedMaterial(const std::string& name);

void RIMP_CreateLight(const std::string& type, const ParameterSet& parameters);

void RIMP_CreateAreaLight(const std::string& type, const ParameterSet& parameters);

void RIMP_CreateModel(const std::string& type, const ParameterSet& parameters);

void RIMP_BeginObject(const std::string& name);

void RIMP_EndObject();

void RIMP_CreateObjectInstance(const std::string& name);

void RIMP_UseSinglePixel(const int pixel[2]);

void RIMP_EndSceneDescription();

} // RayImpact
} // Impact
