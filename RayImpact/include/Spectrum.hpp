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
class SampledSpectrum;

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

static const unsigned int n_SPD_samples = 32;
extern const imp_float SPD_wavelengths[n_SPD_samples];

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

static SampledSpectrum reflectance_white_SPD;
static SampledSpectrum reflectance_red_SPD;
static SampledSpectrum reflectance_green_SPD;
static SampledSpectrum reflectance_blue_SPD;
static SampledSpectrum reflectance_cyan_SPD;
static SampledSpectrum reflectance_magenta_SPD;
static SampledSpectrum reflectance_yellow_SPD;

static SampledSpectrum illumination_white_SPD;
static SampledSpectrum illumination_red_SPD;
static SampledSpectrum illumination_green_SPD;
static SampledSpectrum illumination_blue_SPD;
static SampledSpectrum illumination_cyan_SPD;
static SampledSpectrum illumination_magenta_SPD;
static SampledSpectrum illumination_yellow_SPD;

// Indicator for whether a spectrum is a reflectance or illumination spectrum
enum class SpectrumType {Reflectance, Illumination};

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

	RGBSpectrum(imp_float inital_value);
	
	RGBSpectrum(const CoefficientSpectrum& other);

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

	void computeTristimulusValues(imp_float xyz[3]) const;
	
	imp_float tristimulusY() const;

	void computeRGBValues(imp_float rgb[3]) const;

	RGBSpectrum toRGBSpectrum() const;
};

// Functions on CoefficientSpectrum objects

template <unsigned int n>
inline CoefficientSpectrum<n> sqrt(const CoefficientSpectrum<n>& spectrum)
{
	CoefficientSpectrum<n> result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = std::sqrt(spectrum.coefficients[i]);

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> pow(const CoefficientSpectrum<n>& spectrum, imp_float exponent)
{
	CoefficientSpectrum<n> result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = std::pow(spectrum.coefficients[i], exponent);

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> exp(const CoefficientSpectrum<n>& spectrum)
{
	CoefficientSpectrum<n> result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = std::exp(spectrum.coefficients[i]);

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

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator-(const CoefficientSpectrum& other) const
{
	CoefficientSpectrum result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = coefficients[i] - other.coefficients[i];

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator*(const CoefficientSpectrum& other) const
{
	CoefficientSpectrum result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = coefficients[i]*other.coefficients[i];

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator*(imp_float constant) const
{
	CoefficientSpectrum result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = coefficients[i]*constant;

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator/(const CoefficientSpectrum& other) const
{
	CoefficientSpectrum result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = coefficients[i]/other.coefficients[i];

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator/(imp_float constant) const
{
	CoefficientSpectrum result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = coefficients[i]/constant;

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n> CoefficientSpectrum<n>::operator-() const
{
	CoefficientSpectrum result;

	for (unsigned int i = 0; i < n; i++)
		result.coefficients[i] = -coefficients[i];

	return result;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator+=(const CoefficientSpectrum& other)
{
	for (unsigned int i = 0; i < n; i++)
		coefficients[i] += other.coefficients[i];

	return *this;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator-=(const CoefficientSpectrum& other)
{
	for (unsigned int i = 0; i < n; i++)
		coefficients[i] -= other.coefficients[i];

	return *this;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator*=(const CoefficientSpectrum& other)
{
	for (unsigned int i = 0; i < n; i++)
		coefficients[i] *= other.coefficients[i];

	return *this;
}

template <unsigned int n>
inline CoefficientSpectrum<n>& CoefficientSpectrum<n>::operator/=(const CoefficientSpectrum& other)
{
	for (unsigned int i = 0; i < n; i++)
		coefficients[i] /= other.coefficients[i];

	return *this;
}

template <unsigned int n>
inline bool CoefficientSpectrum<n>::operator==(const CoefficientSpectrum& other) const
{
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

// Spectrum utility functions

// Converts the given tristimulus X, Y and Z values to RGB values
inline void tristimulusToRGB(const imp_float xyz[3], imp_float rgb[])
{
	rgb[0] = 3.240479f*xyz[0] - 1.537150f*xyz[1] - 0.498535f*xyz[2];
    rgb[1] = -0.969256f*xyz[0] + 1.875991f*xyz[1] + 0.041556f*xyz[2];
    rgb[2] = 0.055648f*xyz[0] - 0.204043f*xyz[1] + 1.057311f*xyz[2];
}

// Converts the given RGB values to tristimulus X, Y and Z values
inline void RGBToTristimulus(const imp_float rgb[3], imp_float xyz[])
{
	xyz[0] = 0.412453f*rgb[0] + 0.357580f*rgb[1] + 0.180423f*rgb[2];
    xyz[1] = 0.212671f*rgb[0] + 0.715160f*rgb[1] + 0.072169f*rgb[2];
    xyz[2] = 0.019334f*rgb[0] + 0.119193f*rgb[1] + 0.950227f*rgb[2];
}

// Returns the wavelength [nm] corresponding to the given sample index
inline imp_float sampleWavelength(unsigned int sample_idx)
{
	return ::Impact::lerp(wavelength_samples_start, wavelength_samples_end,
						  static_cast<imp_float>(sample_idx)/static_cast<imp_float>(n_spectral_samples));
}

// Determines whether the given wavelengths are sorted in ascending order
inline bool samplesAreSorted(const imp_float* wavelengths,
							 unsigned int n_samples)
{
	for (unsigned int i = 0; i < n_samples-1; i++)
	{
		if (wavelengths[i+1] < wavelengths[i])
			return false;
	}

	return true;
}

// Sorts the given wavelengths and sample values by wavelength (ascending)
inline void sortSamples(std::vector<imp_float>& wavelengths,
						std::vector<imp_float>& values)
{
	std::sort(wavelengths.begin(), wavelengths.end());

	std::sort(values.begin(), values.end(),
			 [wavelengths](int i, int j)
			 {
			     return (wavelengths[i] < wavelengths[j]);
			 });
}
	
// Computes the average of the sample values in the given wavelength range
inline imp_float averageOfSamples(const imp_float* wavelengths,
								  const imp_float* values,
								  unsigned int n_samples,
								  imp_float start_wavelength, imp_float end_wavelength)
{
	// Simply use endpoint values if the given wavelength range is outside the sampled range
	if (end_wavelength <= wavelengths[0])
		return values[0];
	if (start_wavelength >= wavelengths[n_samples-1])
		return values[n_samples-1];

	// Return the single sample value if there is only one
	if (n_samples == 1)
		return values[0];

	imp_float summed_value = 0.0f;

	// Add endpoint contributions for wavelengths partially outside the sampled range
	if (start_wavelength < wavelengths[0])
		summed_value += values[0]*(wavelengths[0] - start_wavelength);
	if (end_wavelength > wavelengths[n_samples-1])
		summed_value += values[n_samples-1]*(end_wavelength - wavelengths[n_samples-1]);

	unsigned int sample_idx = 0;
	
	// Advance to first sample index inside the wavelength range
	while (start_wavelength > wavelengths[sample_idx])
		sample_idx++;

	// Add sample contributions from each segment inside the wavelength range
	for (; (sample_idx < n_samples-1) && (wavelengths[sample_idx] <= end_wavelength); sample_idx++)
	{
		imp_float segment_start_wavelength = std::max(start_wavelength, wavelengths[sample_idx]);
		imp_float segment_end_wavelength = std::min(end_wavelength, wavelengths[sample_idx+1]);

		summed_value += 0.5f*(::Impact::lerp(values[sample_idx], values[sample_idx+1], (start_wavelength - wavelengths[sample_idx])/(wavelengths[sample_idx+1] - wavelengths[sample_idx])) +
							  ::Impact::lerp(values[sample_idx], values[sample_idx+1], (end_wavelength   - wavelengths[sample_idx])/(wavelengths[sample_idx+1] - wavelengths[sample_idx])))*
						(segment_end_wavelength - segment_start_wavelength);
	}

	return summed_value/(end_wavelength - start_wavelength);
}

} // RayImpact
} // Impact
