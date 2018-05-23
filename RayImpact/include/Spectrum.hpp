#pragma once
#include "precision.hpp"
#include "error.hpp"
#include "math.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <ostream>
#include <sstream>
#include <string>

namespace Impact {
namespace RayImpact {

// Forward declarations
class RGBSpectrum;
class SampledSpectrum;

// Typedefs for generic spectrum class

typedef RGBSpectrum Spectrum;
//typedef SampledSpectrum Spectrum;

typedef Spectrum EnergySpectrum;
typedef Spectrum PowerSpectrum;
typedef Spectrum IntensitySpectrum;
typedef Spectrum RadianceSpectrum;
typedef Spectrum ReflectionSpectrum;
typedef Spectrum TransmissionSpectrum;

// Global variables

static const unsigned int wavelength_samples_start = 400; // First wavelength [nm] sampled in SampledSpectrum
static const unsigned int wavelength_samples_end = 700; // Last wavelength [nm] sampled in SampledSpectrum
static const unsigned int n_spectral_samples = 60; // Total number of samples held by SampledSpectrum

// Indicator for whether a spectrum is a reflectance or illumination spectrum
enum class SpectrumType {Reflectance, Illumination};

// CoefficientSpectrum declarations

template <unsigned int n>
class CoefficientSpectrum {

template <unsigned int n>
friend inline CoefficientSpectrum<n> sqrt(const CoefficientSpectrum<n>& spectrum);

template <unsigned int n>
friend inline CoefficientSpectrum<n> pow(const CoefficientSpectrum<n>& spectrum, imp_float exponent);

template <unsigned int n>
friend inline CoefficientSpectrum<n> exp(const CoefficientSpectrum<n>& spectrum);

template <unsigned int n>
friend inline CoefficientSpectrum<n> lerp(const CoefficientSpectrum<n>& spectrum_1,
                                          const CoefficientSpectrum<n>& spectrum_2,
                                          imp_float weight);

protected:

    CoefficientSpectrum();

    imp_float coefficients[n]; // Coefficients for the SPD basis functions

public:
    static const unsigned int n_coefficients = n;

    CoefficientSpectrum(imp_float initial_value);

    imp_float operator[](unsigned int idx) const;
    imp_float& operator[](unsigned int idx);

    CoefficientSpectrum operator+(const CoefficientSpectrum& other) const;
    CoefficientSpectrum operator-(const CoefficientSpectrum& other) const;
    CoefficientSpectrum operator*(const CoefficientSpectrum& other) const;
    CoefficientSpectrum operator*(imp_float constant) const;
    CoefficientSpectrum operator/(const CoefficientSpectrum& other) const;
    CoefficientSpectrum operator/(imp_float constant) const;

    CoefficientSpectrum operator-() const;

    CoefficientSpectrum& operator+=(const CoefficientSpectrum& other);
    CoefficientSpectrum& operator-=(const CoefficientSpectrum& other);
    CoefficientSpectrum& operator*=(const CoefficientSpectrum& other);
    CoefficientSpectrum& operator/=(const CoefficientSpectrum& other);

    bool operator==(const CoefficientSpectrum& other) const;
    bool operator!=(const CoefficientSpectrum& other) const;

    bool isBlack() const;

    bool hasNaNs() const;

    CoefficientSpectrum clamped(imp_float lower_limit = 0,
                                imp_float upper_limit = IMP_INFINITY) const;
};

// RGBSpectrum declarations

class RGBSpectrum : public CoefficientSpectrum<3> {

private:

public:

    RGBSpectrum();

    RGBSpectrum(imp_float initial_value);

    RGBSpectrum(const imp_float* wavelengths,
                const imp_float* values,
                unsigned int n_samples);

    RGBSpectrum(const imp_float rgb[3],
                SpectrumType type = SpectrumType::Reflectance);

    RGBSpectrum(const CoefficientSpectrum& other);

    explicit RGBSpectrum(const SampledSpectrum& other,
                         SpectrumType type = SpectrumType::Reflectance);

    static RGBSpectrum fromSamples(const imp_float* wavelengths,
                                   const imp_float* values,
                                   unsigned int n_samples);

    static RGBSpectrum fromRGBValues(const imp_float rgb[3],
                                     SpectrumType type = SpectrumType::Reflectance);

    static RGBSpectrum fromTristimulusValues(const imp_float xyz[3],
                                             SpectrumType type = SpectrumType::Reflectance);

    void computeRGBValues(imp_float rgb[3]) const;

    void computeTristimulusValues(imp_float xyz[3]) const;

    imp_float tristimulusY() const;

    const RGBSpectrum& toRGBSpectrum() const;

    SampledSpectrum toSampledSpectrum(SpectrumType type = SpectrumType::Reflectance) const;

	std::string toRGBString() const;
	
	std::string toXYZString() const;
};

// SampledSpectrum declarations

class SampledSpectrum : public CoefficientSpectrum<n_spectral_samples> {

private:

    static void initializeSpectralMatchingCurves();

    static void initializeBaseColorSPDs();

public:

    SampledSpectrum();

    SampledSpectrum(imp_float inital_value);

    SampledSpectrum(const imp_float* wavelengths,
                    const imp_float* values,
                    unsigned int n_samples);

    SampledSpectrum(const imp_float rgb[3],
                    SpectrumType type = SpectrumType::Reflectance);

    SampledSpectrum(const CoefficientSpectrum& other);

    explicit SampledSpectrum(const RGBSpectrum& other,
                             SpectrumType type = SpectrumType::Reflectance);

    static SampledSpectrum fromSamples(const imp_float* wavelengths,
                                       const imp_float* values,
                                       unsigned int n_samples);

    static SampledSpectrum fromRGBValues(const imp_float rgb[3],
                                         SpectrumType type = SpectrumType::Reflectance);

    static SampledSpectrum fromTristimulusValues(const imp_float xyz[3],
                                                 SpectrumType type = SpectrumType::Reflectance);

    void computeRGBValues(imp_float rgb[3]) const;

    void computeTristimulusValues(imp_float xyz[3]) const;

    imp_float tristimulusY() const;

    RGBSpectrum toRGBSpectrum() const;

    const SampledSpectrum& toSampledSpectrum(SpectrumType type = SpectrumType::Reflectance) const;

    static void initialize();

	std::string toRGBString() const;
	
	std::string toXYZString() const;
};

// Spectrum function declarations

void tristimulusToRGB(const imp_float xyz[3], imp_float rgb[3]);

void RGBToTristimulus(const imp_float rgb[3], imp_float xyz[3]);

static imp_float RGBToTristimulusY(const imp_float rgb[3]);

static bool samplesAreSorted(const imp_float* wavelengths,
                             unsigned int n_samples);

static void sortSamples(std::vector<imp_float>& wavelengths,
                        std::vector<imp_float>& values);

template <typename T>
static imp_float averageSamples(const T* wavelengths,
                                const T* values,
                                unsigned int n_samples,
                                T start_wavelength, T end_wavelength);

static imp_float interpolateSamples(const imp_float* wavelengths,
                                    const imp_float* values,
                                    unsigned int n_samples,
                                    imp_float wavelength);

static imp_float sampleWavelength(unsigned int sample_idx);

void computeBlackbodySpectrum(const imp_float* wavelengths,
                              unsigned int n_values,
                              imp_float temperature,
                              imp_float* radiances);

void computeNormalizedBlackbodySpectrum(const imp_float* wavelengths,
                                        unsigned int n_values,
                                        imp_float temperature,
                                        imp_float* radiances);

// CoefficientSpectrum inline method definitions

template <unsigned int n>
inline CoefficientSpectrum<n>::CoefficientSpectrum()
{}

template <unsigned int n>
inline CoefficientSpectrum<n>::CoefficientSpectrum(imp_float initial_value /* = 0 */)
{
    imp_assert(!isNaN(initial_value));
    for (unsigned int i = 0; i < n; i++)
        coefficients[i] = initial_value;
}

template <unsigned int n>
inline imp_float CoefficientSpectrum<n>::operator[](unsigned int idx) const
{
    imp_assert(idx < n);
    return coefficients[idx];
}

template <unsigned int n>
inline imp_float& CoefficientSpectrum<n>::operator[](unsigned int idx)
{
    imp_assert(idx < n);
    return coefficients[idx];
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator+(const CoefficientSpectrum& other) const
{
    CoefficientSpectrum result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = coefficients[i] + other.coefficients[i];

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator-(const CoefficientSpectrum& other) const
{
    CoefficientSpectrum result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = coefficients[i] - other.coefficients[i];

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator*(const CoefficientSpectrum& other) const
{
    CoefficientSpectrum result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = coefficients[i]*other.coefficients[i];

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator*(imp_float constant) const
{
    CoefficientSpectrum result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = coefficients[i]*constant;

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator/(const CoefficientSpectrum& other) const
{
    CoefficientSpectrum result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = coefficients[i]/other.coefficients[i];

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator/(imp_float constant) const
{
    CoefficientSpectrum result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = coefficients[i]/constant;

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator-() const
{
    CoefficientSpectrum result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = -coefficients[i];

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator+=(const CoefficientSpectrum& other)
{
    imp_assert(!other.hasNaNs());

    for (unsigned int i = 0; i < n; i++)
        coefficients[i] += other.coefficients[i];

    return *this;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator-=(const CoefficientSpectrum& other)
{
    imp_assert(!other.hasNaNs());

    for (unsigned int i = 0; i < n; i++)
        coefficients[i] -= other.coefficients[i];

    return *this;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator*=(const CoefficientSpectrum& other)
{
    imp_assert(!other.hasNaNs());

    for (unsigned int i = 0; i < n; i++)
        coefficients[i] *= other.coefficients[i];

    return *this;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator/=(const CoefficientSpectrum& other)
{
    imp_assert(!other.hasNaNs());

    for (unsigned int i = 0; i < n; i++)
        coefficients[i] /= other.coefficients[i];

    return *this;
}

template <unsigned int n>
inline bool CoefficientSpectrum<n>::operator==(const CoefficientSpectrum& other) const
{
    imp_assert(!other.hasNaNs());

    for (unsigned int i = 0; i < n; i++)
    {
        if (coefficients[i] != other.coefficients[i])
            return false;
    }

    return true;
}

template <unsigned int n>
inline bool CoefficientSpectrum<n>::operator!=(const CoefficientSpectrum& other) const
{
    return !(*this == other);
}

template <unsigned int n>
inline bool CoefficientSpectrum<n>::isBlack() const
{
    for (unsigned int i = 0; i < n; i++)
    {
        if (coefficients[i] != 0)
            return false;
    }

    return true;
}

template <unsigned int n>
inline bool CoefficientSpectrum<n>::hasNaNs() const
{
    for (unsigned int i = 0; i < n; i++)
    {
        if (isNaN(coefficients[i]))
            return true;
    }

    return false;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::clamped(imp_float lower_limit /* = 0 */,
                                                              imp_float upper_limit /* = IMP_INFINITY */) const
{
    CoefficientSpectrum<n> result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = clamp(coefficients[i], lower_limit, upper_limit);

    return result;
}

// CoefficientSpectrum inline function definitions

template <unsigned int n>
inline CoefficientSpectrum<n> sqrt(const CoefficientSpectrum<n>& spectrum)
{
    CoefficientSpectrum<n> result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = std::sqrt(spectrum.coefficients[i]);

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> pow(const CoefficientSpectrum<n>& spectrum, imp_float exponent)
{
    CoefficientSpectrum<n> result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = std::pow(spectrum.coefficients[i], exponent);

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> exp(const CoefficientSpectrum<n>& spectrum)
{
    CoefficientSpectrum<n> result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = std::exp(spectrum.coefficients[i]);

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> lerp(const CoefficientSpectrum<n>& spectrum_1,
                                   const CoefficientSpectrum<n>& spectrum_2,
                                   imp_float weight)
{
    CoefficientSpectrum<n> result;

    for (unsigned int i = 0; i < n; i++)
        result.coefficients[i] = ::Impact::lerp(spectrum_1.coefficients[i], spectrum_2.coefficients[i], weight);

    imp_assert(!result.hasNaNs());

    return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> operator*(imp_float constant, const CoefficientSpectrum<n>& spectrum)
{
    return spectrum*constant;
}

// RGBSpectrum inline method definitions

inline RGBSpectrum::RGBSpectrum()
    : CoefficientSpectrum<3>::CoefficientSpectrum()
{}

inline RGBSpectrum::RGBSpectrum(imp_float initial_value)
    : CoefficientSpectrum<3>::CoefficientSpectrum(initial_value)
{}

inline RGBSpectrum::RGBSpectrum(const imp_float* wavelengths,
								const imp_float* values,
								unsigned int n_samples)
{
    *this = RGBSpectrum::fromSamples(wavelengths, values, n_samples);
}

inline RGBSpectrum::RGBSpectrum(const imp_float rgb[3],
								SpectrumType type /* = SpectrumType::Reflectance */)
{
    *this = RGBSpectrum::fromRGBValues(rgb, type);
}

inline RGBSpectrum::RGBSpectrum(const CoefficientSpectrum& other)
    : CoefficientSpectrum<3>::CoefficientSpectrum(other)
{}

inline const RGBSpectrum& RGBSpectrum::toRGBSpectrum() const
{
    return *this;
}

inline SampledSpectrum RGBSpectrum::toSampledSpectrum(SpectrumType type /* = SpectrumType::Reflectance */) const
{
    return SampledSpectrum(*this, type);
}

inline std::string RGBSpectrum::toRGBString() const
{
    std::ostringstream stream;
	stream << "{R: " << coefficients[0] << ", G: " << coefficients[1] << ", B: " << coefficients[2] << "}";
    return stream.str();
}

inline std::string RGBSpectrum::toXYZString() const
{
	imp_float xyz[3];
    RGBToTristimulus(coefficients, xyz);
    std::ostringstream stream;
	stream << "{X: " << xyz[0] << ", Y: " << xyz[1] << ", Z: " << xyz[2] << "}";
    return stream.str();
}

// SampledSpectrum inline method definitions

inline SampledSpectrum::SampledSpectrum()
    : CoefficientSpectrum<n_spectral_samples>::CoefficientSpectrum()
{}

inline SampledSpectrum::SampledSpectrum(imp_float initial_value)
    : CoefficientSpectrum<n_spectral_samples>::CoefficientSpectrum(initial_value)
{}

inline SampledSpectrum::SampledSpectrum(const imp_float* wavelengths,
										const imp_float* values,
										unsigned int n_samples)
{
    *this = SampledSpectrum::fromSamples(wavelengths, values, n_samples);
}


inline SampledSpectrum::SampledSpectrum(const imp_float rgb[3],
										SpectrumType type /* = SpectrumType::Reflectance */)
{
    *this = SampledSpectrum::fromRGBValues(rgb, type);
}

inline SampledSpectrum::SampledSpectrum(const CoefficientSpectrum& other)
    : CoefficientSpectrum<n_spectral_samples>::CoefficientSpectrum(other)
{}

inline RGBSpectrum SampledSpectrum::toRGBSpectrum() const
{
    return RGBSpectrum(*this);
}

inline const SampledSpectrum& SampledSpectrum::toSampledSpectrum(SpectrumType type /* = SpectrumType::Reflectance */) const
{
    return *this;
}

// Initializes SampledSpectrum objects required for conversion between different color representations
inline void SampledSpectrum::initialize()
{
    initializeSpectralMatchingCurves();
    initializeBaseColorSPDs();
}

inline std::string SampledSpectrum::toRGBString() const
{
    return toRGBSpectrum().toRGBString();
}

inline std::string SampledSpectrum::toXYZString() const
{
	return toRGBSpectrum().toXYZString();
}

// RGBSpectrum inline function definitions

inline std::ostream& operator<<(std::ostream& stream, const RGBSpectrum& spectrum)
{
	stream << spectrum.toRGBString();
    return stream;
}

// SampledSpectrum inline function definitions

inline std::ostream& operator<<(std::ostream& stream, const SampledSpectrum& spectrum)
{
	stream << spectrum.toRGBString();
    return stream;
}

} // RayImpact
} // Impact
