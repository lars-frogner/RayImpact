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
};

extern Options RIMP_OPTIONS; // Global rendering options

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

void RIMP_UseRotation(const Vector3F& axis, imp_float angle);

void RIMP_UseScaling(const Vector3F& scaling);

void RIMP_UseWorldToCamera(const Point3F& camera_position,
						   const Vector3F& up_vector,
						   const Point3F& look_point);

void RIMP_UseTransformation(const imp_float matrix_elements[16]);

void RIMP_UseConcatenated(const imp_float matrix_elements[16]);

void RIMP_DefineCoordinateSystem(const std::string& name);

void RIMP_UseCoordinateSystem(const std::string& name);

void RIMP_DefineMedium(const std::string& name, const ParameterSet& parameters);

void RIMP_UseMediumInterface(const std::string& inside_medium_name, const std::string& outside_medium_name);

void RIMP_SetTransformationTimes(imp_float start_time, imp_float end_time);

void RIMP_SetAccelerationStructure(const std::string& type, const ParameterSet& parameters);

void RIMP_SetSampler(const std::string& type, const ParameterSet& parameters);

void RIMP_SetFilter(const std::string& type, const ParameterSet& parameters);

void RIMP_SetCamera(const std::string& type, const ParameterSet& parameters);

void RIMP_SetCameraSensor(const std::string& type, const ParameterSet& parameters);

void RIMP_SetIntegrator(const std::string& type, const ParameterSet& parameters);

void RIMP_BeginSceneDescription();

void RIMP_BeginAttribute();

void RIMP_EndAttribute();

void RIMP_BeginTransformation();

void RIMP_EndTransformation();

void RIMP_CreateModel(const std::string& type, const ParameterSet& parameters);

void RIMP_BeginObject(const std::string& name);

void RIMP_EndObject();

void RIMP_CreateObjectInstance(const std::string& name);

void RIMP_EndSceneDescription();

} // RayImpact
} // Impact
