#include "ParameterSet.hpp"
#include "error.hpp"

namespace Impact {
namespace RayImpact {

// ParameterSet macros

#define add_parameter(type, param_vector) \
    param_vector.emplace_back(new Parameter<type>(name, std::move(values), n_values))

#define remove_parameter(param_vector) \
	for (size_t i = 0; i < param_vector.size(); i++) \
	{ \
		if (param_vector[i]->name == name) \
		{ \
			param_vector.erase(param_vector.begin() + i); \
			return true; \
		} \
	} \
	return false
	
#define get_single_parameter_value(param_vector) \
	for (const auto& param : param_vector) \
	{ \
		if (param->name == name && param->n_values == 1) \
		{ \
			param->was_looked_up = true; \
			return param->values[0]; \
		} \
	} \
	return default_value
	
#define get_parameter_values(param_vector) \
	for (const auto& param : param_vector) \
	{ \
		if (param->name == name) \
		{ \
			param->was_looked_up = true; \
			*n_values = param->n_values; \
			return param->values.get(); \
		} \
	} \
	return nullptr

// ParameterSet method implmentations

void ParameterSet::addBoolParameter(const std::string& name, std::unique_ptr<bool[]> values, unsigned int n_values)
{
	removeBoolParameter(name);
	add_parameter(bool, bool_params);
}

void ParameterSet::addIntParameter(const std::string& name, std::unique_ptr<int[]> values, unsigned int n_values)
{
	removeIntParameter(name);
	add_parameter(int, int_params);
}

void ParameterSet::addFloatParameter(const std::string& name, std::unique_ptr<imp_float[]> values, unsigned int n_values)
{
	removeFloatParameter(name);
	add_parameter(imp_float, float_params);
}

void ParameterSet::addStringParameter(const std::string& name, std::unique_ptr<std::string[]> values, unsigned int n_values)
{
	removeStringParameter(name);
	add_parameter(std::string, string_params);
}

void ParameterSet::addTextureNameParameter(const std::string& name, std::unique_ptr<std::string[]> values, unsigned int n_values)
{
	removeTextureNameParameter(name);
	add_parameter(std::string, texture_name_params);
}

void ParameterSet::addPoint2FParameter(const std::string& name, std::unique_ptr<Point2F[]> values, unsigned int n_values)
{
	removePoint2FParameter(name);
	add_parameter(Point2F, point2f_params);
}

void ParameterSet::addVector2FParameter(const std::string& name, std::unique_ptr<Vector2F[]> values, unsigned int n_values)
{
	removeVector2FParameter(name);
	add_parameter(Vector2F, vector2f_params);
}

void ParameterSet::addPoint3FParameter(const std::string& name, std::unique_ptr<Point3F[]> values, unsigned int n_values)
{
	removePoint3FParameter(name);
	add_parameter(Point3F, point3f_params);
}

void ParameterSet::addVector3FParameter(const std::string& name, std::unique_ptr<Vector3F[]> values, unsigned int n_values)
{
	removeVector3FParameter(name);
	add_parameter(Vector3F, vector3f_params);
}

void ParameterSet::addNormal3FParameter(const std::string& name, std::unique_ptr<Normal3F[]> values, unsigned int n_values)
{
	removeNormal3FParameter(name);
	add_parameter(Normal3F, normal3f_params);
}

void ParameterSet::addSpectrumParameter(const std::string& name, std::unique_ptr<Spectrum[]> values, unsigned int n_values)
{
	removeSpectrumParameter(name);
	add_parameter(Spectrum, spectrum_params);
}

bool ParameterSet::removeBoolParameter(const std::string& name)
{
	remove_parameter(bool_params);
}

bool ParameterSet::removeIntParameter(const std::string& name)
{
	remove_parameter(int_params);
}

bool ParameterSet::removeFloatParameter(const std::string& name)
{
	remove_parameter(float_params);
}

bool ParameterSet::removeStringParameter(const std::string& name)
{
	remove_parameter(string_params);
}

bool ParameterSet::removeTextureNameParameter(const std::string& name)
{
	remove_parameter(texture_name_params);
}

bool ParameterSet::removePoint2FParameter(const std::string& name)
{
	remove_parameter(point2f_params);
}

bool ParameterSet::removeVector2FParameter(const std::string& name)
{
	remove_parameter(vector2f_params);
}

bool ParameterSet::removePoint3FParameter(const std::string& name)
{
	remove_parameter(point3f_params);
}

bool ParameterSet::removeVector3FParameter(const std::string& name)
{
	remove_parameter(vector3f_params);
}

bool ParameterSet::removeNormal3FParameter(const std::string& name)
{
	remove_parameter(normal3f_params);
}

bool ParameterSet::removeSpectrumParameter(const std::string& name)
{
	remove_parameter(spectrum_params);
}

bool ParameterSet::getSingleBoolValue(const std::string& name, bool default_value) const
{
	get_single_parameter_value(bool_params);
}

int ParameterSet::getSingleIntValue(const std::string& name, int default_value) const
{
	get_single_parameter_value(int_params);
}
imp_float ParameterSet::getSingleFloatValue(const std::string& name, imp_float default_value) const
{
	get_single_parameter_value(float_params);
}
const std::string& ParameterSet::getSingleStringValue(const std::string& name, const std::string& default_value) const
{
	get_single_parameter_value(string_params);
}
const std::string& ParameterSet::getSingleTextureNameValue(const std::string& name, const std::string& default_value) const
{
	get_single_parameter_value(texture_name_params);
}
const Point2F& ParameterSet::getSinglePoint2FValue(const std::string& name, const Point2F& default_value) const
{
	get_single_parameter_value(point2f_params);
}
const Vector2F& ParameterSet::getSingleVector2FValue(const std::string& name, const Vector2F& default_value) const
{
	get_single_parameter_value(vector2f_params);
}
const Point3F& ParameterSet::getSinglePoint3FValue(const std::string& name, const Point3F& default_value) const
{
	get_single_parameter_value(point3f_params);
}
const Vector3F& ParameterSet::getSingleVector3FValue(const std::string& name, const Vector3F& default_value) const
{
	get_single_parameter_value(vector3f_params);
}
const Normal3F& ParameterSet::getSingleNormal3FValue(const std::string& name, const Normal3F& default_value) const
{
	get_single_parameter_value(normal3f_params);
}
const Spectrum& ParameterSet::getSingleSpectrumValue(const std::string& name, const Spectrum& default_value) const
{
	get_single_parameter_value(spectrum_params);
}

const bool* ParameterSet::getBoolValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(bool_params);
}

const int* ParameterSet::getIntValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(int_params);
}
const imp_float* ParameterSet::getFloatValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(float_params);
}
const std::string* ParameterSet::getStringValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(string_params);
}
const std::string* ParameterSet::getTextureNameValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(texture_name_params);
}
const Point2F* ParameterSet::getPoint2FValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(point2f_params);
}
const Vector2F* ParameterSet::getVector2FValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(vector2f_params);
}
const Point3F* ParameterSet::getPoint3FValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(point3f_params);
}
const Vector3F* ParameterSet::getVector3FValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(vector3f_params);
}
const Normal3F* ParameterSet::getNormal3FValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(normal3f_params);
}
const Spectrum* ParameterSet::getSpectrumValues(const std::string& name, unsigned int* n_values) const
{
	get_parameter_values(spectrum_params);
}

void ParameterSet::constructSpectrumParameterFromRGB(const std::string& name, const imp_float* rgb_values,
													 unsigned int n_rgb_triples)
{
	std::unique_ptr<Spectrum[]> spectra(new Spectrum[n_rgb_triples]);

	for (unsigned int i = 0; i < n_rgb_triples; i++)
	{
		spectra[i] = Spectrum::fromRGBValues(rgb_values + 3*i);
	}

	addSpectrumParameter(name, std::move(spectra), n_rgb_triples);
}

void ParameterSet::constructSpectrumParameterFromTristimulus(const std::string& name, const imp_float* xyz_values,
															 unsigned int n_xyz_triples)
{
	std::unique_ptr<Spectrum[]> spectra(new Spectrum[n_xyz_triples]);

	for (unsigned int i = 0; i < n_xyz_triples; i++)
	{
		spectra[i] = Spectrum::fromTristimulusValues(xyz_values + 3*i);
	}
	
	addSpectrumParameter(name, std::move(spectra), n_xyz_triples);
}

void ParameterSet::constructSpectrumParameterFromSamples(const std::string& name, const imp_float* wavelengths,
														 const imp_float* values, const int* n_samples,	
														 unsigned int n_sample_arrays)
{
	std::unique_ptr<Spectrum[]> spectra(new Spectrum[n_sample_arrays]);

	unsigned int cumulative_n_samples = 0;

	for (unsigned int i = 0; i < n_sample_arrays; i++)
	{
		spectra[i] = Spectrum::fromSamples(wavelengths + cumulative_n_samples,
										   values + cumulative_n_samples,
										   n_samples[i]);
		cumulative_n_samples += n_samples[i];
	}
	
	addSpectrumParameter(name, std::move(spectra), n_sample_arrays);
}

/*void ParameterSet::constructSpectrumParameterFromSampleFiles(const std::string& name, const std::string* filenames,
															 unsigned int n_files)
{
	for (unsigned int i = 0; i < n_files; i++)
	{

	}
}*/

void ParameterSet::warnAboutUnusedParameters() const
{
	for (const auto& param : bool_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("bool parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : int_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("int parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : float_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("float parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : string_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("string parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : texture_name_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("texture name parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : point2f_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("Point2F parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : vector2f_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("Vector2F parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : point3f_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("Point3F parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : vector3f_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("Vector3F parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : normal3f_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("Normal3F parameter \"%s\" is unused", param->name.c_str());
	}

	for (const auto& param : spectrum_params)
	{
		if (!param->was_looked_up)
			printWarningMessage("Spectrum parameter \"%s\" is unused", param->name.c_str());
	}
}

void ParameterSet::clearParameters()
{
	bool_params.clear();
	int_params.clear();
	float_params.clear();
	string_params.clear();
	texture_name_params.clear();
	point2f_params.clear();
	vector2f_params.clear();
	point3f_params.clear();
	vector3f_params.clear();
	normal3f_params.clear();
	spectrum_params.clear();
}

} // Impact
} // RayImpact
