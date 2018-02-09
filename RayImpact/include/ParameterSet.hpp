#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include "Spectrum.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace Impact {
namespace RayImpact {

// Forward declarations
template <typename T>
class Texture;

// Parameter declarations

template <typename T>
class Parameter {

public:

    const std::string name; // The name of the parameter
    const std::unique_ptr<T[]> values; // The values of parameter
    const unsigned int n_values; // The number of values in the parameter
    mutable bool was_looked_up; // Whether the values of the parameter have been looked up

    Parameter(const std::string& name,
              std::unique_ptr<T[]> param_values,
              unsigned int n_values);
};

// ParameterSet declarations

class ParameterSet {

private:

    std::vector< std::shared_ptr< Parameter<bool> > > bool_params;
    std::vector< std::shared_ptr< Parameter<int> > > int_params;
    std::vector< std::shared_ptr< Parameter<imp_float> > > float_params;
    std::vector< std::shared_ptr< Parameter<std::string> > > string_params;
    std::vector< std::shared_ptr< Parameter<std::string> > > texture_name_params;
    std::vector< std::shared_ptr< Parameter<Point2F> > > point2f_params;
    std::vector< std::shared_ptr< Parameter<Vector2F> > > vector2f_params;
    std::vector< std::shared_ptr< Parameter<Point3F> > > point3f_params;
    std::vector< std::shared_ptr< Parameter<Vector3F> > > vector3f_params;
    std::vector< std::shared_ptr< Parameter<Normal3F> > > normal3f_params;
    std::vector< std::shared_ptr< Parameter<Spectrum> > > spectrum_params;

public:

    void addBoolParameter(const std::string& name, std::unique_ptr<bool[]> values, unsigned int n_values);
    void addIntParameter(const std::string& name, std::unique_ptr<int[]> values, unsigned int n_values);
    void addFloatParameter(const std::string& name, std::unique_ptr<imp_float[]> values, unsigned int n_values);
    void addStringParameter(const std::string& name, std::unique_ptr<std::string[]> values, unsigned int n_values);
    void addTextureNameParameter(const std::string& name, std::unique_ptr<std::string[]> values, unsigned int n_values);
    void addPoint2FParameter(const std::string& name, std::unique_ptr<Point2F[]> values, unsigned int n_values);
    void addVector2FParameter(const std::string& name, std::unique_ptr<Vector2F[]> values, unsigned int n_values);
    void addPoint3FParameter(const std::string& name, std::unique_ptr<Point3F[]> values, unsigned int n_values);
    void addVector3FParameter(const std::string& name, std::unique_ptr<Vector3F[]> values, unsigned int n_values);
    void addNormal3FParameter(const std::string& name, std::unique_ptr<Normal3F[]> values, unsigned int n_values);
    void addSpectrumParameter(const std::string& name, std::unique_ptr<Spectrum[]> values, unsigned int n_values);

    bool removeBoolParameter(const std::string& name);
    bool removeIntParameter(const std::string& name);
    bool removeFloatParameter(const std::string& name);
    bool removeStringParameter(const std::string& name);
    bool removeTextureNameParameter(const std::string& name);
    bool removePoint2FParameter(const std::string& name);
    bool removeVector2FParameter(const std::string& name);
    bool removePoint3FParameter(const std::string& name);
    bool removeVector3FParameter(const std::string& name);
    bool removeNormal3FParameter(const std::string& name);
    bool removeSpectrumParameter(const std::string& name);

    bool getSingleBoolValue(const std::string& name, bool default_value) const;
    int getSingleIntValue(const std::string& name, int default_value) const;
    imp_float getSingleFloatValue(const std::string& name, imp_float default_value) const;
    const std::string& getSingleStringValue(const std::string& name, const std::string& default_value) const;
    const std::string& getSingleTextureNameValue(const std::string& name, const std::string& default_value) const;
    const Point2F& getSinglePoint2FValue(const std::string& name, const Point2F& default_value) const;
    const Vector2F& getSingleVector2FValue(const std::string& name, const Vector2F& default_value) const;
    const Point3F& getSinglePoint3FValue(const std::string& name, const Point3F& default_value) const;
    const Vector3F& getSingleVector3FValue(const std::string& name, const Vector3F& default_value) const;
    const Normal3F& getSingleNormal3FValue(const std::string& name, const Normal3F& default_value) const;
    const Spectrum& getSingleSpectrumValue(const std::string& name, const Spectrum& default_value) const;

    const bool* getBoolValues(const std::string& name, unsigned int* n_values) const;
    const int* getIntValues(const std::string& name, unsigned int* n_values) const;
    const imp_float* getFloatValues(const std::string& name, unsigned int* n_values) const;
    const std::string* getStringValues(const std::string& name, unsigned int* n_values) const;
    const std::string* getTextureNameValues(const std::string& name, unsigned int* n_values) const;
    const Point2F* getPoint2FValues(const std::string& name, unsigned int* n_values) const;
    const Vector2F* getVector2FValues(const std::string& name, unsigned int* n_values) const;
    const Point3F* getPoint3FValues(const std::string& name, unsigned int* n_values) const;
    const Vector3F* getVector3FValues(const std::string& name, unsigned int* n_values) const;
    const Normal3F* getNormal3FValues(const std::string& name, unsigned int* n_values) const;
    const Spectrum* getSpectrumValues(const std::string& name, unsigned int* n_values) const;

    void constructSpectrumParameterFromRGB(const std::string& name, const imp_float* rgb_values,
                                           unsigned int n_rgb_triples);

    void constructSpectrumParameterFromTristimulus(const std::string& name, const imp_float* xyz_values,
                                                   unsigned int n_xyz_triples);

    void constructSpectrumParameterFromSamples(const std::string& name, const imp_float* wavelengths,
                                               const imp_float* values, const int* n_samples,
                                               unsigned int n_sample_arrays);

    //void constructSpectrumParameterFromSampleFiles(const std::string& name, const std::string* filenames,
    //                                                 unsigned int n_files);

    void warnAboutUnusedParameters() const;

    void clearParameters();
};

// TextureParameterSet declarations

class TextureParameterSet {

private:

    std::map< std::string, std::shared_ptr< Texture<imp_float> > >& float_textures;
    std::map< std::string, std::shared_ptr< Texture<Spectrum> > >& spectrum_textures;
    const ParameterSet& geometry_parameters;
    const ParameterSet& material_parameters;

public:

    TextureParameterSet(std::map< std::string, std::shared_ptr< Texture<imp_float> > >& float_textures,
                        std::map< std::string, std::shared_ptr< Texture<Spectrum> > >& spectrum_textures,
                        const ParameterSet& geometry_parameters,
                        const ParameterSet& material_parameters);

    std::shared_ptr< Texture<imp_float> > getFloatTexture(const std::string& name, imp_float default_value) const;
    std::shared_ptr< Texture<imp_float> > getFloatTexture(const std::string& name) const;

    std::shared_ptr< Texture<Spectrum> > getSpectrumTexture(const std::string& name, const Spectrum& default_value) const;
    std::shared_ptr< Texture<Spectrum> > getSpectrumTexture(const std::string& name) const;

    bool getSingleBoolValue(const std::string& name, bool default_value) const;
    int getSingleIntValue(const std::string& name, int default_value) const;
    imp_float getSingleFloatValue(const std::string& name, imp_float default_value) const;
    const std::string& getSingleStringValue(const std::string& name, const std::string& default_value) const;
    const Point2F& getSinglePoint2FValue(const std::string& name, const Point2F& default_value) const;
    const Vector2F& getSingleVector2FValue(const std::string& name, const Vector2F& default_value) const;
    const Point3F& getSinglePoint3FValue(const std::string& name, const Point3F& default_value) const;
    const Vector3F& getSingleVector3FValue(const std::string& name, const Vector3F& default_value) const;
    const Normal3F& getSingleNormal3FValue(const std::string& name, const Normal3F& default_value) const;
    const Spectrum& getSingleSpectrumValue(const std::string& name, const Spectrum& default_value) const;

    void warnAboutUnusedParameters() const;
};

// Parameter method implementations

template <typename T>
Parameter<T>::Parameter(const std::string& name,
                          std::unique_ptr<T[]> param_values,
                        unsigned int n_values)
    : name(name),
      values(std::move(param_values)),
      n_values(n_values),
      was_looked_up(false)
{}

} // Impact
} // RayImpact
