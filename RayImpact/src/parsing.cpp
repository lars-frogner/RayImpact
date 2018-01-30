#include "parsing.h"
#include "parsing.tab.h"
#include "precision.hpp"
#include "error.hpp"
#include "string_util.hpp"
#include "geometry.hpp"
#include "api.hpp"
#include "ParameterSet.hpp"
#include <vector>
#include <string>
#include <memory>

using namespace Impact;
using namespace RayImpact;

#define create_single_value_array_ptr(type, arr) \
	imp_assert(!arr.empty()); \
	unsigned int n_values = (unsigned int)arr.size(); \
	std::unique_ptr<type[]> value_ptr(new type[n_values]); \
	do { \
		for (unsigned int i = 0; i < n_values; i++) \
			value_ptr[i] = arr[i]; \
	} while (false)

#define create_double_value_array_ptr(type, arr) \
	imp_assert(arr.size() % 2 == 0 && !arr.empty()); \
	unsigned int n_values = (unsigned int)(arr.size()/2); \
	std::unique_ptr<type[]> value_ptr(new type[n_values]); \
	do { \
		for (unsigned int i = 0; i < n_values; i++) \
			value_ptr[i] = type(arr[2*i], arr[2*i + 1]); \
	} while (false)

#define create_triple_value_array_ptr(type, arr) \
	imp_assert(arr.size() % 3 == 0 && !arr.empty()); \
	unsigned int n_values = (unsigned int)(arr.size()/3); \
	std::unique_ptr<type[]> value_ptr(new type[n_values]); \
	do { \
		for (unsigned int i = 0; i < n_values; i++) \
			value_ptr[i] = type(arr[3*i], arr[3*i + 1], arr[3*i + 2]); \
	} while (false)

#define find_next_bool_arg(value, is_single) \
	value = parameter_set.getBoolValues(nextPositionalArgumentID(), &n_values); \
	if (!value || (is_single && n_values != 1)) \
	{ \
		printErrorMessage("argument number %d (bool%s) to \"%s\" is missing or has invalid type. Ignoring call.", positional_arg, (is_single)? "" : " array", name); \
		resetArguments(); \
		return; \
	} \
	else

#define find_next_int_arg(value, is_single) \
	value = parameter_set.getIntValues(nextPositionalArgumentID(), &n_values); \
	if (!value || (is_single && n_values != 1)) \
	{ \
		printErrorMessage("argument number %d (int%s) to \"%s\" is missing or has invalid type. Ignoring call.", positional_arg, (is_single)? "" : " array", name); \
		resetArguments(); \
		return; \
	} \
	else

#define find_next_float_arg(value, is_single) \
	value = parameter_set.getFloatValues(nextPositionalArgumentID(), &n_values); \
	if (!value || (is_single && n_values != 1)) \
	{ \
		printErrorMessage("argument number %d (float%s) to \"%s\" is missing or has invalid type. Ignoring call.", positional_arg, (is_single)? "" : " array", name); \
		resetArguments(); \
		return; \
	} \
	else

#define find_next_string_arg(value, is_single) \
	value = parameter_set.getStringValues(nextPositionalArgumentID(), &n_values); \
	if (!value || (is_single && n_values != 1)) \
	{ \
		printErrorMessage("argument number %d (string%s) to \"%s\" is missing or has invalid type. Ignoring call.", positional_arg, (is_single)? "" : " array", name); \
		resetArguments(); \
		return; \
	} \
	else

#define find_next_vector2_arg(value) \
	value = parameter_set.getVector2FValues(nextPositionalArgumentID(), &n_values); \
	if (!value || n_values != 1) \
	{ \
		printErrorMessage("argument number %d (float pair) to \"%s\" is missing or has invalid type. Ignoring call.", positional_arg, name); \
		resetArguments(); \
		return; \
	} \
	else

#define find_next_vector3_arg(value) \
	value = parameter_set.getVector3FValues(nextPositionalArgumentID(), &n_values); \
	if (!value || n_values != 1) \
	{ \
		printErrorMessage("argument number %d (float triplet) to \"%s\" is missing or has invalid type. Ignoring call.", positional_arg, name); \
		resetArguments(); \
		return; \
	} \
	else

static std::string parsing_output_filename("stdout");

static unsigned int positional_arg = 0;

static std::vector<bool> bool_array;
static std::vector<int> int_array;
static std::vector<imp_float> float_array;
static std::vector<std::string> string_array;
static std::string next_positional_argument_id;

static ParameterSet parameter_set;

const char* nextPositionalArgumentID()
{
	next_positional_argument_id = formatString("#%d", ++positional_arg);
	return next_positional_argument_id.c_str();
}

void addBoolElement(int value)
{
	bool_array.push_back(value);
}

void addIntElement(int value)
{
	int_array.push_back(value);
}

void addFloatElement(double value)
{
	float_array.push_back((imp_float)value);
}

void addStringElement(const char* value)
{
	string_array.push_back(std::string(value));
}

void clearBoolElements()
{
	bool_array.clear();
}

void clearIntElements()
{
	int_array.clear();
}

void clearFloatElements()
{
	float_array.clear();
}

void clearStringElements()
{
	string_array.clear();
}

void clearAllElements()
{
	clearBoolElements();
	clearIntElements();
	clearFloatElements();
	clearStringElements();
}

void addBoolParameter(const char* name)
{
	create_single_value_array_ptr(bool, bool_array);
	parameter_set.addBoolParameter(std::string(name), std::move(value_ptr), n_values);
	clearBoolElements();
}

void addIntParameter(const char* name)
{
	create_single_value_array_ptr(int, int_array);
	parameter_set.addIntParameter(std::string(name), std::move(value_ptr), n_values);
	clearIntElements();
}

void addFloatParameter(const char* name)
{
	create_single_value_array_ptr(imp_float, float_array);
	parameter_set.addFloatParameter(std::string(name), std::move(value_ptr), n_values);
	clearFloatElements();
}

void addStringParameter(const char* name)
{
	create_single_value_array_ptr(std::string, string_array);
	parameter_set.addStringParameter(std::string(name), std::move(value_ptr), n_values);
	clearStringElements();
}

void addPoint2FParameter(const char* name)
{
	create_double_value_array_ptr(Point2F, float_array);
	parameter_set.addPoint2FParameter(std::string(name), std::move(value_ptr), n_values);
	clearFloatElements();
}

void addVector2FParameter(const char* name)
{
	create_double_value_array_ptr(Vector2F, float_array);
	parameter_set.addVector2FParameter(std::string(name), std::move(value_ptr), n_values);
	clearFloatElements();
}

void addPoint3FParameter(const char* name)
{
	create_triple_value_array_ptr(Point3F, float_array);
	parameter_set.addPoint3FParameter(std::string(name), std::move(value_ptr), n_values);
	clearFloatElements();
}

void addVector3FParameter(const char* name)
{
	create_triple_value_array_ptr(Vector3F, float_array);
	parameter_set.addVector3FParameter(std::string(name), std::move(value_ptr), n_values);
	clearFloatElements();
}

void addNormal3FParameter(const char* name)
{
	create_triple_value_array_ptr(Normal3F, float_array);
	parameter_set.addNormal3FParameter(std::string(name), std::move(value_ptr), n_values);
	clearFloatElements();
}

void addRGBSpectrumParameter(const char* name)
{
	imp_assert(float_array.size() % 3 == 0 && !float_array.empty());
	parameter_set.constructSpectrumParameterFromRGB(std::string(name), float_array.data(), (unsigned int)(float_array.size()/3));
	clearFloatElements();
}

void addTristimulusSpectrumParameter(const char* name)
{
	imp_assert(float_array.size() % 3 == 0 && !float_array.empty());
	parameter_set.constructSpectrumParameterFromTristimulus(std::string(name), float_array.data(), (unsigned int)(float_array.size()/3));
	clearFloatElements();
}

void addSampledSpectrumParameter(const char* name)
{
	imp_assert(float_array.size() % 2 == 0 && !float_array.empty());
	
	unsigned int n_samples_total;

	if (int_array.empty())
	{
		n_samples_total = (unsigned int)(float_array.size()/2);
		addIntElement((int)n_samples_total);
	}
	else
	{
		n_samples_total = 0;

		for (size_t i = 0; i < int_array.size(); i++)
			n_samples_total += (unsigned int)(int_array[i]);

		if (n_samples_total != (unsigned int)(float_array.size()/2))
		{
			printErrorMessage("the sum of the SPD sample numbers is not consistent with the number of given samples. Ignoring parameter.");
			clearFloatElements();
			clearIntElements();
			return;
		}
	}

	std::vector<imp_float> wavelengths;
	std::vector<imp_float> values;

	wavelengths.reserve(n_samples_total);
	values.reserve(n_samples_total);

	for (size_t i = 0; i < n_samples_total; i++)
	{
		wavelengths.push_back(float_array[2*i]);
		values.push_back(float_array[2*i + 1]);
	}

	parameter_set.constructSpectrumParameterFromSamples(std::string(name),
														wavelengths.data(),
														values.data(),
														int_array.data(),
														(unsigned int)int_array.size());
	clearFloatElements();
	clearIntElements();
}

void resetArguments()
{
	parameter_set.clearParameters();
	positional_arg = 0;
}

void callAPIFunction(const char* name)
{
	std::string function_name(name);
	positional_arg = 0;
	unsigned int n_values;

	printInfoMessage("Calling \"%s\"", name);

	if (function_name == "SetOption")
	{
		const std::string* option;
		const std::string* value;
		find_next_string_arg(option, true);
		find_next_string_arg(value, true);
		RIMP_SetOption(*option, *value);
	}
	else if (function_name == "Initialize")
	{
		RIMP_Initialize();
	}
	else if (function_name == "Cleanup")
	{
		printErrorMessage("Cleanup is performed automatically. Ignoring call.");
	}
	else if (function_name == "SetActiveTransformationsToAll")
	{
		RIMP_SetActiveTransformationsToAll();
	}
	else if (function_name == "SetActiveTransformationsToInitial")
	{
		RIMP_SetActiveTransformationsToInitial();
	}
	else if (function_name == "SetActiveTransformationsToFinal")
	{
		RIMP_SetActiveTransformationsToFinal();
	}
	else if (function_name == "UseIdentity")
	{
		RIMP_UseIdentity();
	}
	else if (function_name == "UseTranslation")
	{
		const Vector3F* displacement;
		find_next_vector3_arg(displacement);
		RIMP_UseTranslation(*displacement);
	}
	else if (function_name == "UseRotation")
	{
		const Vector3F* axis;
		const imp_float* angle;
		find_next_vector3_arg(axis);
		find_next_float_arg(angle, true);
		RIMP_UseRotation(*axis, *angle);
	}
	else if (function_name == "UseScaling")
	{
		const Vector3F* scaling;
		find_next_vector3_arg(scaling);
		RIMP_UseScaling(*scaling);
	}
	else if (function_name == "UseWorldToCamera")
	{
		const Vector3F* camera_position;
		const Vector3F* up_vector;
		const Vector3F* look_point;
		find_next_vector3_arg(camera_position);
		find_next_vector3_arg(up_vector);
		find_next_vector3_arg(look_point);
		RIMP_UseWorldToCamera(Point3F(camera_position->x, camera_position->y, camera_position->z),
							  *up_vector,
							  Point3F(look_point->x, look_point->y, look_point->z));
	}
	else if (function_name == "UseTransformation")
	{
		const imp_float* matrix_elements;
		find_next_float_arg(matrix_elements, false);
		if (n_values != 16)
		{
			printErrorMessage("argument number %d (float array) to \"%s\" must have size 16. Ignoring call.", positional_arg, name);
			resetArguments();
			return;
		}
		RIMP_UseTransformation(matrix_elements);
	}
	else if (function_name == "UseConcatenated")
	{
		const imp_float* matrix_elements;
		find_next_float_arg(matrix_elements, false);
		if (n_values != 16)
		{
			printErrorMessage("argument number %d (float array) to \"%s\" must have size 16. Ignoring call.", positional_arg, name);
			resetArguments();
			return;
		}
		RIMP_UseConcatenated(matrix_elements);
	}
	else if (function_name == "DefineCoordinateSystem")
	{
		const std::string* name;
		find_next_string_arg(name, true);
		RIMP_DefineCoordinateSystem(*name);
	}
	else if (function_name == "UseCoordinateSystem")
	{
		const std::string* name;
		find_next_string_arg(name, true);
		RIMP_UseCoordinateSystem(*name);
	}
	else if (function_name == "DefineMedium")
	{
		const std::string* name;
		find_next_string_arg(name, true);
		RIMP_DefineMedium(*name, parameter_set);
	}
	else if (function_name == "UseMediumInterface")
	{
		const std::string* inside_medium_name;
		const std::string* outside_medium_name;
		find_next_string_arg(inside_medium_name, true);
		find_next_string_arg(outside_medium_name, true);
		RIMP_UseMediumInterface(*inside_medium_name, *outside_medium_name);
	}
	else if (function_name == "SetTransformationTimes")
	{
		const imp_float* start_time;
		const imp_float* end_time;
		find_next_float_arg(start_time, true);
		find_next_float_arg(end_time, true);
		RIMP_SetTransformationTimes(*start_time, *end_time);
	}
	else if (function_name == "SetAccelerationStructure")
	{
		const std::string* type;
		find_next_string_arg(type, true);
		RIMP_SetAccelerationStructure(*type, parameter_set);
	}
	else if (function_name == "SetSampler")
	{
		const std::string* type;
		find_next_string_arg(type, true);
		RIMP_SetSampler(*type, parameter_set);
	}
	else if (function_name == "SetFilter")
	{
		const std::string* type;
		find_next_string_arg(type, true);
		RIMP_SetFilter(*type, parameter_set);
	}
	else if (function_name == "SetCamera")
	{
		const std::string* type;
		find_next_string_arg(type, true);
		RIMP_SetCamera(*type, parameter_set);
	}
	else if (function_name == "SetCameraSensor")
	{
		const std::string* type;
		find_next_string_arg(type, true);
		RIMP_SetCameraSensor(*type, parameter_set);
	}
	else if (function_name == "SetIntegrator")
	{
		const std::string* type;
		find_next_string_arg(type, true);
		RIMP_SetIntegrator(*type, parameter_set);
	}
	else if (function_name == "BeginSceneDescription")
	{
		RIMP_BeginSceneDescription();
	}
	else if (function_name == "BeginAttribute")
	{
		RIMP_BeginAttribute();
	}
	else if (function_name == "EndAttribute")
	{
		RIMP_EndAttribute();
	}
	else if (function_name == "BeginTransformation")
	{
		RIMP_BeginTransformation();
	}
	else if (function_name == "EndTransformation")
	{
		RIMP_EndTransformation();
	}
	else if (function_name == "CreateModel")
	{
		const std::string* type;
		find_next_string_arg(type, true);
		RIMP_CreateModel(*type, parameter_set);
	}
	else if (function_name == "BeginObject")
	{
		const std::string* name;
		find_next_string_arg(name, true);
		RIMP_BeginObject(*name);
	}
	else if (function_name == "EndObject")
	{
		RIMP_EndObject();
	}
	else if (function_name == "CreateObjectInstance")
	{
		const std::string* name;
		find_next_string_arg(name, true);
		RIMP_CreateObjectInstance(*name);
	}
	else if (function_name == "EndSceneDescription")
	{
		RIMP_EndSceneDescription();
	}
	else
	{
		printErrorMessage("invalid statement \"%s\". Ignoring.", name);
		resetArguments();
		return;
	}
	
	parameter_set.warnAboutUnusedParameters();
	resetArguments();
}

int parseFile(const char* filename)
{
	if (std::string(filename) != "stdin")
	{
		if (fopen_s(&yyin, filename, "r") != 0)
		{
			printErrorMessage("could not open \"%s\" for parsing. Ignoring.", filename);
			return 0;
		}
	}
		
	if (parsing_output_filename != "stdout")
	{
		if (fopen_s(&yyout, parsing_output_filename.c_str(), "w") != 0)
		{
			printErrorMessage("could not open \"%s\" for writing parsing output. Using stdout.", parsing_output_filename.c_str());
			yyout = nullptr;
		}
	}

    yyparse();
	
	if (yyin)
		fclose(yyin);
		
	if (yyout)
		fclose(yyout);

	return 1;
}

namespace Impact {
namespace RayImpact {

void parseSceneDescriptionFiles(const std::vector<std::string>& filenames)
{
	for (const std::string& filename : filenames)
	{
		parseFile(filename.c_str());
	}

	if (needsCleanup())
		RIMP_Cleanup();
}

} // RayImpact
} // Impact
