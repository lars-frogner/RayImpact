#pragma once
#include "precision.hpp"
#include "geometry.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetDistribution declarations

class MicrofacetDistribution {

protected:

	const bool sample_visible_area;

	MicrofacetDistribution(bool sample_visible_area);

public:

    virtual imp_float areaWithMicroNormal(const Vector3F& micro_normal) const = 0;

    virtual imp_float maskedAreaFraction(const Vector3F& direction) const = 0;

    imp_float visibleFraction(const Vector3F& direction) const;

    imp_float visibleFraction(const Vector3F& direction_1,
                              const Vector3F& direction_2) const;

	virtual Vector3F sampleMicroNormal(const Vector3F& outgoing_direction,
									   const Point2F& uniform_sample) const = 0;

	imp_float pdf(const Vector3F& outgoing_direction,
				  const Vector3F& micro_normal) const;
};

// BeckmannDistribution declarations

class BeckmannDistribution : public MicrofacetDistribution {

private:
    imp_float slope_deviation_x; // sqrt(2) times the RMS of the x-part of the slope distribution
    imp_float slope_deviation_y; // sqrt(2) times the RMS of the y-part of the slope distribution
    imp_float inv_slope_x_sq, inv_slope_y_sq; // Precomputed values for evaluations

public:

    BeckmannDistribution(imp_float slope_deviation_x,
                         imp_float slope_deviation_y,
						 bool sample_visible_area);

    static imp_float roughnessToDeviation(imp_float roughness);

    imp_float areaWithMicroNormal(const Vector3F& micro_normal) const;

    imp_float maskedAreaFraction(const Vector3F& direction) const;

	Vector3F sampleMicroNormal(const Vector3F& outgoing_direction,
							   const Point2F& uniform_sample) const;
};

// TrowbridgeReitzDistribution declarations

class TrowbridgeReitzDistribution : public MicrofacetDistribution {

private:
    imp_float slope_deviation_x; // sqrt(2) times the RMS of the x-part of the slope distribution
    imp_float slope_deviation_y; // sqrt(2) times the RMS of the y-part of the slope distribution
    imp_float inv_slope_x_sq, inv_slope_y_sq; // Precomputed values for evaluations

public:

    TrowbridgeReitzDistribution(imp_float slope_deviation_x,
                                imp_float slope_deviation_y,
								bool sample_visible_area);

    static imp_float roughnessToDeviation(imp_float roughness);

    imp_float areaWithMicroNormal(const Vector3F& micro_normal) const;

    imp_float maskedAreaFraction(const Vector3F& direction) const;

	Vector3F sampleMicroNormal(const Vector3F& outgoing_direction,
							   const Point2F& uniform_sample) const;
};

// MicrofacetDistribution inline method definitions

inline MicrofacetDistribution::MicrofacetDistribution(bool sample_visible_area)
	: sample_visible_area(sample_visible_area)
{}

inline imp_float MicrofacetDistribution::visibleFraction(const Vector3F& direction) const
{
    return 1.0f/(1 + maskedAreaFraction(direction));
}

inline imp_float MicrofacetDistribution::visibleFraction(const Vector3F& direction_1,
														 const Vector3F& direction_2) const
{
    return 1.0f/(1 + maskedAreaFraction(direction_1) + maskedAreaFraction(direction_2));
}

} // RayImpact
} // Impact
