#include "ParameterSet.hpp"
#include "Texture.hpp"
#include "ConstantTexture.hpp"
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
	do { \
    for (const auto& param : param_vector) \
    { \
        if (param->name == name && param->n_values == 1) \
        { \
            param->was_looked_up = true; \
            return param->values[0]; \
        } \
    } \
	} while (false)

#define get_parameter_values(param_vector) \
	do { \
    for (const auto& param : param_vector) \
    { \
        if (param->name == name) \
        { \
            param->was_looked_up = true; \
            *n_values = param->n_values; \
            return param->values.get(); \
        } \
    } \
	} while (false)

// ParameterSet method definitions

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

void ParameterSet::addNumParameter(const std::string& name, std::unique_ptr<imp_float[]> values, unsigned int n_values)
{
    removeNumParameter(name);
	
	bool might_be_int = true;
	std::unique_ptr<int[]> int_values(new int[n_values]);

	for (unsigned int i = 0; i < n_values; i++)
	{
		int_values[i] = (int)(values[i]);
		if (values[i] != (imp_float)(int_values[i]))
		{
			might_be_int = false;
			break;
		}
	}

    add_parameter(imp_float, float_num_params);

	if (might_be_int)
	    int_num_params.emplace_back(new Parameter<int>(name, std::move(int_values), n_values));
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

void ParameterSet::addPairParameter(const std::string& name, std::unique_ptr<Vector2F[]> values, unsigned int n_values)
{
    removePairParameter(name);
    add_parameter(Vector2F, pair_params);
}

void ParameterSet::addTripleParameter(const std::string& name, std::unique_ptr<Vector3F[]> values, unsigned int n_values)
{
    removeTripleParameter(name);
    add_parameter(Vector3F, triple_params);
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

bool ParameterSet::removeNumParameter(const std::string& name)
{
    remove_parameter(float_num_params);
    remove_parameter(int_num_params);
}

bool ParameterSet::removeStringParameter(const std::string& name)
{
    remove_parameter(string_params);
}

bool ParameterSet::removeTextureNameParameter(const std::string& name)
{
    remove_parameter(texture_name_params);
}

bool ParameterSet::removePairParameter(const std::string& name)
{
    remove_parameter(pair_params);
}

bool ParameterSet::removeTripleParameter(const std::string& name)
{
    remove_parameter(triple_params);
}

bool ParameterSet::removeSpectrumParameter(const std::string& name)
{
    remove_parameter(spectrum_params);
}

bool ParameterSet::getSingleBoolValue(const std::string& name, bool default_value) const
{
    get_single_parameter_value(bool_params);
	return default_value;
}

int ParameterSet::getSingleIntValue(const std::string& name, int default_value) const
{
    get_single_parameter_value(int_params);

	for (const auto& param : int_num_params)
    {
        if (param->name == name && param->n_values == 1)
        {
            param->was_looked_up = true;

			for (const auto& flt_param : float_num_params)
				if (flt_param->name == name && flt_param->n_values == 1)
				{
					flt_param->was_looked_up = true;
					break;
				}

            return param->values[0];
        }
    }

    return default_value;
}

imp_float ParameterSet::getSingleFloatValue(const std::string& name, imp_float default_value) const
{
    get_single_parameter_value(float_params);
    get_single_parameter_value(float_num_params);
    return default_value;
}

const std::string& ParameterSet::getSingleStringValue(const std::string& name, const std::string& default_value) const
{
    get_single_parameter_value(string_params);
	return default_value;
}

const std::string& ParameterSet::getSingleTextureNameValue(const std::string& name, const std::string& default_value) const
{
    get_single_parameter_value(texture_name_params);
	return default_value;
}

const Vector2F& ParameterSet::getSinglePairValue(const std::string& name, const Vector2F& default_value) const
{
    get_single_parameter_value(pair_params);
	return default_value;
}

const Vector3F& ParameterSet::getSingleTripleValue(const std::string& name, const Vector3F& default_value) const
{
    get_single_parameter_value(triple_params);
	return default_value;
}

Spectrum ParameterSet::getSingleSpectrumValue(const std::string& name, const Spectrum& default_value) const
{
    get_single_parameter_value(spectrum_params);

	for (const auto& param : triple_params)
    {
        if (param->name == name && param->n_values == 1)
        {
            param->was_looked_up = true;
            const Vector3F& val = param->values[0];
			imp_float rgb[3] = {val.x, val.y, val.z};
			return Spectrum(rgb);
        }
    }

	return default_value;
}

const bool* ParameterSet::getBoolValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(bool_params);
	return nullptr;
}

const int* ParameterSet::getIntValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(int_params);

	for (const auto& param : int_num_params)
    {
        if (param->name == name)
        {
            param->was_looked_up = true;
            *n_values = param->n_values;

			for (const auto& flt_param : float_num_params)
				if (flt_param->name == name)
				{
					flt_param->was_looked_up = true;
					break;
				}

            return param->values.get();
        }
    }

    return nullptr;
}

const imp_float* ParameterSet::getFloatValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(float_params);
    get_parameter_values(float_num_params);
    return nullptr;
}

const std::string* ParameterSet::getStringValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(string_params);
	return nullptr;
}

const std::string* ParameterSet::getTextureNameValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(texture_name_params);
	return nullptr;
}

const Vector2F* ParameterSet::getPairValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(pair_params);
	return nullptr;
}

const Vector3F* ParameterSet::getTripleValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(triple_params);
	return nullptr;
}

const Spectrum* ParameterSet::getSpectrumValues(const std::string& name, unsigned int* n_values) const
{
    get_parameter_values(spectrum_params);
	return nullptr;
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

    for (const auto& param : float_num_params)
    {
        if (!param->was_looked_up)
            printWarningMessage("parameter \"%s\" is unused", param->name.c_str());
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

    for (const auto& param : pair_params)
    {
        if (!param->was_looked_up)
            printWarningMessage("Pair parameter \"%s\" is unused", param->name.c_str());
    }

    for (const auto& param : triple_params)
    {
        if (!param->was_looked_up)
            printWarningMessage("Triple parameter \"%s\" is unused", param->name.c_str());
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
    int_num_params.clear();
    float_num_params.clear();
    string_params.clear();
    texture_name_params.clear();
	pair_params.clear();
	triple_params.clear();
    spectrum_params.clear();
}

// TextureParameterSet method definitions

TextureParameterSet::TextureParameterSet(std::map< std::string, std::shared_ptr< Texture<imp_float> > >& float_textures,
                                         std::map< std::string, std::shared_ptr< Texture<Spectrum> > >& spectrum_textures,
                                         const ParameterSet& geometry_parameters,
                                         const ParameterSet& material_parameters)
    : float_textures(float_textures),
      spectrum_textures(spectrum_textures),
      geometry_parameters(geometry_parameters),
      material_parameters(material_parameters)
{}

std::shared_ptr< Texture<imp_float> > TextureParameterSet::getFloatTexture(const std::string& name, imp_float default_value) const
{
    std::string texture_name = geometry_parameters.getSingleTextureNameValue(name, "");

    if (texture_name == "")
        texture_name = material_parameters.getSingleTextureNameValue(texture_name, "");

    if (texture_name != "")
    {
        auto float_texture = float_textures.find(texture_name);

        if (float_texture != float_textures.end())
            return float_texture->second;
        else
            printErrorMessage("couldn't find float texture named \"%s\" for parameter \"%s\"",
                              texture_name.c_str(), name.c_str());
    }

    imp_float value = material_parameters.getSingleFloatValue(name, default_value);
    value = geometry_parameters.getSingleFloatValue(name, value);

    return std::make_shared< ConstantTexture<imp_float> >(value);
}

std::shared_ptr< Texture<imp_float> > TextureParameterSet::getFloatTexture(const std::string& name) const
{
    std::shared_ptr< Texture<imp_float> > texture;

    std::string texture_name = geometry_parameters.getSingleTextureNameValue(name, "");

    if (texture_name == "")
        texture_name = material_parameters.getSingleTextureNameValue(texture_name, "");

    if (texture_name != "")
    {
        auto float_texture = float_textures.find(texture_name);

        if (float_texture != float_textures.end())
            texture = float_texture->second;
    }

    return texture;
}

std::shared_ptr< Texture<Spectrum> > TextureParameterSet::getSpectrumTexture(const std::string& name, const Spectrum& default_value) const
{
    std::string texture_name = geometry_parameters.getSingleTextureNameValue(name, "");

    if (texture_name == "")
        texture_name = material_parameters.getSingleTextureNameValue(texture_name, "");

    if (texture_name != "")
    {
        auto spectrum_texture = spectrum_textures.find(texture_name);

        if (spectrum_texture != spectrum_textures.end())
            return spectrum_texture->second;
        else
            printErrorMessage("Couldn't find spectrum texture named \"%s\" for parameter \"%s\"",
                              texture_name.c_str(), name.c_str());
    }

    Spectrum value = material_parameters.getSingleSpectrumValue(name, default_value);
    value = geometry_parameters.getSingleSpectrumValue(name, value);

    return std::make_shared< ConstantTexture<Spectrum> >(value);
}

std::shared_ptr< Texture<Spectrum> > TextureParameterSet::getSpectrumTexture(const std::string& name) const
{
    std::shared_ptr< Texture<Spectrum> > texture;

    std::string texture_name = geometry_parameters.getSingleTextureNameValue(name, "");

    if (texture_name == "")
        texture_name = material_parameters.getSingleTextureNameValue(texture_name, "");

    if (texture_name != "")
    {
        auto spectrum_texture = spectrum_textures.find(texture_name);

        if (spectrum_texture != spectrum_textures.end())
            texture = spectrum_texture->second;
    }

    return texture;
}

bool TextureParameterSet::getSingleBoolValue(const std::string& name, bool default_value) const
{
    return geometry_parameters.getSingleBoolValue(name, material_parameters.getSingleBoolValue(name, default_value));
}

int TextureParameterSet::getSingleIntValue(const std::string& name, int default_value) const
{
    return geometry_parameters.getSingleIntValue(name, material_parameters.getSingleIntValue(name, default_value));
}

imp_float TextureParameterSet::getSingleFloatValue(const std::string& name, imp_float default_value) const
{
    return geometry_parameters.getSingleFloatValue(name, material_parameters.getSingleFloatValue(name, default_value));
}

const std::string& TextureParameterSet::getSingleStringValue(const std::string& name, const std::string& default_value) const
{
    return geometry_parameters.getSingleStringValue(name, material_parameters.getSingleStringValue(name, default_value));
}

const Vector2F& TextureParameterSet::getSinglePairValue(const std::string& name, const Vector2F& default_value) const
{
    return geometry_parameters.getSinglePairValue(name, material_parameters.getSinglePairValue(name, default_value));
}

const Vector3F& TextureParameterSet::getSingleTripleValue(const std::string& name, const Vector3F& default_value) const
{
    return geometry_parameters.getSingleTripleValue(name, material_parameters.getSingleTripleValue(name, default_value));
}

Spectrum TextureParameterSet::getSingleSpectrumValue(const std::string& name, const Spectrum& default_value) const
{
    return geometry_parameters.getSingleSpectrumValue(name, material_parameters.getSingleSpectrumValue(name, default_value));
}

void TextureParameterSet::warnAboutUnusedParameters() const
{
    geometry_parameters.warnAboutUnusedParameters();
    material_parameters.warnAboutUnusedParameters();
}

} // Impact
} // RayImpact
