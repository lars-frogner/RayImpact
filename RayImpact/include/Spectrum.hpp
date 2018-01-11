#pragma once
#include "precision.hpp"
#include "error.hpp"
#include "math.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace Impact {
namespace RayImpact {

// Forward declarations
class RGBSpectrum;
class SampledSpectrum;

// Typedef for generic spectrum class
typedef RGBSpectrum Spectrum;
//typedef SampledSpectrum Spectrum;

// Global variables

static const unsigned int wavelength_samples_start = 400; // First wavelength [nm] sampled in SampledSpectrum
static const unsigned int wavelength_samples_end = 700; // Last wavelength [nm] sampled in SampledSpectrum
static const unsigned int n_spectral_samples = 60; // Total number of samples held by SampledSpectrum

static const unsigned int n_CIE_samples = 471; // Number of samples of the CIE spectral matching curves
extern const imp_float CIE_wavelengths[n_CIE_samples]; // Wavelengths [nm] for samples of the CIE spectral matching curves
extern const imp_float CIE_X_values[n_CIE_samples]; // Values of the CIE X spectral matching curve
extern const imp_float CIE_Y_values[n_CIE_samples]; // Values of the CIE Y spectral matching curve
extern const imp_float CIE_Z_values[n_CIE_samples]; // Values of the CIE > spectral matching curve

static const imp_float CIE_Y_integral = 106.856895f; // Integral of the CIE Y spectral matching curve

static SampledSpectrum CIE_X; // CIE X spectral matching curve
static SampledSpectrum CIE_Y; // CIE Y spectral matching curve
static SampledSpectrum CIE_Z; // CIE Z spectral matching curve

static const unsigned int n_SPD_samples = 32; // Number of samples of the base color SPDs
extern const imp_float SPD_wavelengths[n_SPD_samples]; // Wavelengths [nm] for samples of the base color SPDs

extern const imp_float reflectance_white_SPD_values[n_SPD_samples];
extern const imp_float reflectance_red_SPD_values[n_SPD_samples];
extern const imp_float reflectance_green_SPD_values[n_SPD_samples];
extern const imp_float reflectance_blue_SPD_values[n_SPD_samples];
extern const imp_float reflectance_cyan_SPD_values[n_SPD_samples];
extern const imp_float reflectance_magenta_SPD_values[n_SPD_samples];
extern const imp_float reflectance_yellow_SPD_values[n_SPD_samples];

extern const imp_float illumination_white_SPD_values[n_SPD_samples];
extern const imp_float illumination_red_SPD_values[n_SPD_samples];
extern const imp_float illumination_green_SPD_values[n_SPD_samples];
extern const imp_float illumination_blue_SPD_values[n_SPD_samples];
extern const imp_float illumination_cyan_SPD_values[n_SPD_samples];
extern const imp_float illumination_magenta_SPD_values[n_SPD_samples];
extern const imp_float illumination_yellow_SPD_values[n_SPD_samples];

static SampledSpectrum reflectance_white_SPD; // White reflectance SPD
static SampledSpectrum reflectance_red_SPD; // Red reflectance SPD
static SampledSpectrum reflectance_green_SPD; // Green reflectance SPD
static SampledSpectrum reflectance_blue_SPD; // Blue reflectance SPD
static SampledSpectrum reflectance_cyan_SPD; // Cyan reflectance SPD
static SampledSpectrum reflectance_magenta_SPD; // Magenta reflectance SPD
static SampledSpectrum reflectance_yellow_SPD; // Yellow reflectance SPD

static SampledSpectrum illumination_white_SPD; // White illumination SPD
static SampledSpectrum illumination_red_SPD; // Red illumination SPD
static SampledSpectrum illumination_green_SPD; // Green illumination SPD
static SampledSpectrum illumination_blue_SPD; // Blue illumination SPD
static SampledSpectrum illumination_cyan_SPD; // Cyan illumination SPD
static SampledSpectrum illumination_magenta_SPD; // Magenta illumination SPD
static SampledSpectrum illumination_yellow_SPD; // Yellow illumination SPD

// Indicator for whether a spectrum is a reflectance or illumination spectrum
enum class SpectrumType {Reflectance, Illumination};

// Spectrum utility functions

static bool samplesAreSorted(const imp_float* wavelengths,
							 unsigned int n_samples);

static void sortSamples(std::vector<imp_float>& wavelengths,
						std::vector<imp_float>& values);
	
static imp_float averageSamples(const imp_float* wavelengths,
							    const imp_float* values,
								unsigned int n_samples,
								imp_float start_wavelength, imp_float end_wavelength);
	
static imp_float interpolateSamples(const imp_float* wavelengths,
									const imp_float* values,
									unsigned int n_samples,
									imp_float wavelength);

static imp_float sampleWavelength(unsigned int sample_idx);

static void tristimulusToRGB(const imp_float xyz[3], imp_float rgb[3]);

static void RGBToTristimulus(const imp_float rgb[3], imp_float xyz[3]);

static imp_float RGBToTristimulusY(const imp_float rgb[3]);

// CoefficientSpectrum declarations

template <unsigned int n>
class CoefficientSpectrum {

friend CoefficientSpectrum sqrt(const CoefficientSpectrum& spectrum);

friend CoefficientSpectrum pow(const CoefficientSpectrum& spectrum, imp_float exponent);

friend CoefficientSpectrum exp(const CoefficientSpectrum& spectrum);

friend CoefficientSpectrum lerp(const CoefficientSpectrum& spectrum_1,
								const CoefficientSpectrum& spectrum_2,
								imp_float weight);

protected:
	
	CoefficientSpectrum();

	imp_float coefficients[n]; // Coefficients for the SPD basis functions

public:
	static const unsigned int n_coefficients = n;
	
	CoefficientSpectrum();
	
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
	
	CoefficientSpectrum clamped(imp_float lower_limit = 0.0f,
								imp_float upper_limit = IMP_INFINITY) const;
};

// RGBSpectrum declarations

class RGBSpectrum : public CoefficientSpectrum<3> {

private:

	RGBSpectrum();

public:

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
};

// SampledSpectrum declarations

class SampledSpectrum : public CoefficientSpectrum<n_spectral_samples> {

private:
	
	SampledSpectrum();

	static void initializeSpectralMatchingCurves();

	static void initializeBaseColorSPDs();

public:

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
};

// Functions on CoefficientSpectrum objects

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

// CoefficientSpectrum method implementations

template <unsigned int n>
inline CoefficientSpectrum<n>::CoefficientSpectrum()
{}

template <unsigned int n>
inline CoefficientSpectrum<n>::CoefficientSpectrum(imp_float initial_value /* = 0.0f */)
{
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
		if (coefficients[i] != 0.0f)
			return false;
	}

	return true;
}

template <unsigned int n>
inline bool CoefficientSpectrum<n>::hasNaNs() const
{
	for (unsigned int i = 0; i < n; i++)
	{
		if (std::isnan(coefficients[i]))
			return true;
	}

	return false;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::clamped(imp_float lower_limit /* = 0.0f */,
															  imp_float upper_limit /* = IMP_INFINITY */) const
{
	CoefficientSpectrum<n> result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = clamp(coefficients[i], lower_limit, upper_limit);

	return result;
}

} // RayImpact
} // Impact
