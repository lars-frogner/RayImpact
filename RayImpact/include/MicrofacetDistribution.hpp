#pragma once
#include "precision.hpp"
#include "geometry.hpp"

namespace Impact {
namespace RayImpact {

// MicrofacetDistribution declarations

class MicrofacetDistribution {

public:

    virtual imp_float areaWithMicroNormal(const Vector3F& micro_normal) const = 0;

    virtual imp_float maskedAreaFraction(const Vector3F& direction) const = 0;

    imp_float visibleFraction(const Vector3F& direction) const;

    imp_float visibleFraction(const Vector3F& direction_1,
                              const Vector3F& direction_2) const;
};

// BeckmannDistribution declarations

class BeckmannDistribution : public MicrofacetDistribution {

private:
    imp_float slope_deviation_x; // sqrt(2) times the RMS of the x-part of the slope distribution
    imp_float slope_deviation_y; // sqrt(2) times the RMS of the y-part of the slope distribution
    imp_float inv_slope_x_sq, inv_slope_y_sq; // Precomputed values for evaluations

public:

    BeckmannDistribution(imp_float slope_deviation_x,
                         imp_float slope_deviation_y);

    static imp_float roughnessToDeviation(imp_float roughness);

    imp_float areaWithMicroNormal(const Vector3F& micro_normal) const;

    imp_float maskedAreaFraction(const Vector3F& direction) const;
};

// TrowbridgeReitzDistribution declarations

class TrowbridgeReitzDistribution : public MicrofacetDistribution {

private:
    imp_float slope_deviation_x; // sqrt(2) times the RMS of the x-part of the slope distribution
    imp_float slope_deviation_y; // sqrt(2) times the RMS of the y-part of the slope distribution
    imp_float inv_slope_x_sq, inv_slope_y_sq; // Precomputed values for evaluations

public:

    TrowbridgeReitzDistribution(imp_float slope_deviation_x,
                                imp_float slope_deviation_y);

    static imp_float roughnessToDeviation(imp_float roughness);

    imp_float areaWithMicroNormal(const Vector3F& micro_normal) const;

    imp_float maskedAreaFraction(const Vector3F& direction) const;
};

} // RayImpact
} // Impact
