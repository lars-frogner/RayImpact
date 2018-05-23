#include "MicrofacetDistribution.hpp"
#include "math.hpp"
#include "BSDF.hpp"
#include "spherical.hpp"
#include <algorithm>
#include <cmath>

namespace Impact {
namespace RayImpact {

// MicrofacetDistribution method definitions

imp_float MicrofacetDistribution::pdf(const Vector3F& outgoing_direction,
									  const Vector3F& micro_normal) const
{
	if (sample_visible_area)
	{
		return areaWithMicroNormal(micro_normal)*
			   maskedAreaFraction(outgoing_direction)*
			   outgoing_direction.absDot(micro_normal)/
			   absCosTheta(outgoing_direction);
	}
	else
	{
		return areaWithMicroNormal(micro_normal)*absCosTheta(micro_normal);
	}
}

// BeckmannDistribution method definitions

BeckmannDistribution::BeckmannDistribution(imp_float slope_deviation_x,
                                           imp_float slope_deviation_y,
										   bool sample_visible_area)
    : MicrofacetDistribution::MicrofacetDistribution(sample_visible_area),
	  slope_deviation_x(slope_deviation_x),
      slope_deviation_y(slope_deviation_y)
{
    inv_slope_x_sq = 1.0f/(slope_deviation_x*slope_deviation_x);
    inv_slope_y_sq = 1.0f/(slope_deviation_y*slope_deviation_y);
}

imp_float BeckmannDistribution::roughnessToDeviation(imp_float roughness)
{
    roughness = std::max<imp_float>(1e-3, roughness);
    imp_float x = std::log(roughness);
    return 1.62142f + 0.819955f*x + 0.1734f*x*x + 0.0171201f*x*x*x + 0.000640711f*x*x*x*x;
}

imp_float BeckmannDistribution::areaWithMicroNormal(const Vector3F& micro_normal) const
{
    imp_float tan_sq_theta = tanSquaredTheta(micro_normal);

    if (std::isinf(tan_sq_theta))
        return 0;

    imp_float cos_sq_theta = cosSquaredTheta(micro_normal);

    return std::exp(-tan_sq_theta*(cosSquaredPhi(micro_normal)*inv_slope_x_sq +
                                   sinSquaredPhi(micro_normal)*inv_slope_y_sq))/
           (IMP_PI*slope_deviation_x*slope_deviation_y*cos_sq_theta*cos_sq_theta);
}

imp_float BeckmannDistribution::maskedAreaFraction(const Vector3F& direction) const
{
    imp_float abs_tan_theta = std::abs(tanTheta(direction));

    if (std::isinf(abs_tan_theta))
        return 0;

    imp_float slope_deviation = std::sqrt(cosSquaredPhi(direction)*slope_deviation_x*slope_deviation_x +
                                          sinSquaredPhi(direction)*slope_deviation_y*slope_deviation_y);

    imp_float a = 1.0f/(slope_deviation*abs_tan_theta);

    if (a >= 1.6f)
        return 0;

    return (1 - 1.259f*a + 0.396f*a*a)/(3.535f*a + 2.181f*a*a);
}

Vector3F BeckmannDistribution::sampleMicroNormal(const Vector3F& outgoing_direction,
												 const Point2F& uniform_sample) const
{
	if (sample_visible_area)
	{
		printSevereMessage("Sampling only visible area of BeckmannDistribution has not been implemented.");
		return Vector3F();
	}
	else
	{
		imp_float tan2_theta;
		imp_float phi;

		if (slope_deviation_x == slope_deviation_y)
		{
			imp_float log_sample = (uniform_sample.x > 0)? std::log(uniform_sample.x) : 0;
			tan2_theta = -slope_deviation_x*slope_deviation_x*log_sample;
			phi = IMP_TWO_PI*uniform_sample.y;
		}
		else
		{
			printSevereMessage("Sampling of anisotropic BeckmannDistribution has not been implemented.");
		}

		imp_float cos_theta = 1/std::sqrt(1 + tan2_theta);
		imp_float sin_theta = std::sqrt(std::max<imp_float>(0, 1 - cos_theta*cos_theta));

		Vector3F micro_normal = sphericalToDirection(cos_theta, sin_theta, phi);
		if (!sameHemisphere(outgoing_direction, micro_normal)) micro_normal.reverse();

		return micro_normal;
	}
}

// TrowbridgeReitzDistribution method definitions

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(imp_float slope_deviation_x,
                                                         imp_float slope_deviation_y,
														 bool sample_visible_area)
    : MicrofacetDistribution::MicrofacetDistribution(sample_visible_area),
	  slope_deviation_x(slope_deviation_x),
      slope_deviation_y(slope_deviation_y)
{
    inv_slope_x_sq = 1.0f/(slope_deviation_x*slope_deviation_x);
    inv_slope_y_sq = 1.0f/(slope_deviation_y*slope_deviation_y);
}

imp_float TrowbridgeReitzDistribution::roughnessToDeviation(imp_float roughness)
{
    roughness = std::max<imp_float>(1e-3, roughness);
    imp_float x = std::log(roughness);
    return 1.62142f + 0.819955f*x + 0.1734f*x*x + 0.0171201f*x*x*x + 0.000640711f*x*x*x*x;
}

imp_float TrowbridgeReitzDistribution::areaWithMicroNormal(const Vector3F& micro_normal) const
{
    imp_float tan_sq_theta = tanSquaredTheta(micro_normal);

    if (std::isinf(tan_sq_theta))
        return 0;

    imp_float cos_sq_theta = cosSquaredTheta(micro_normal);

    imp_float denom_factor = (1 + tan_sq_theta*(cosSquaredPhi(micro_normal)*inv_slope_x_sq +
                                                sinSquaredPhi(micro_normal)*inv_slope_y_sq));

    return 1.0f/(IMP_PI*slope_deviation_x*slope_deviation_y*cos_sq_theta*cos_sq_theta*denom_factor*denom_factor);
}

imp_float TrowbridgeReitzDistribution::maskedAreaFraction(const Vector3F& direction) const
{
    imp_float abs_tan_theta = std::abs(tanTheta(direction));

    if (std::isinf(abs_tan_theta))
        return 0;

    imp_float slope_deviation = std::sqrt(cosSquaredPhi(direction)*slope_deviation_x*slope_deviation_x +
                                          sinSquaredPhi(direction)*slope_deviation_y*slope_deviation_y);

    imp_float slope_deviation_tan_theta = slope_deviation*abs_tan_theta;

    return 0.5f*(std::sqrt(1 + slope_deviation_tan_theta*slope_deviation_tan_theta) - 1);
}

Vector3F TrowbridgeReitzDistribution::sampleMicroNormal(const Vector3F& outgoing_direction,
														const Point2F& uniform_sample) const
{
	printSevereMessage("Sampling of TrowbridgeReitzDistribution has not been implemented.");
	return Vector3F();
}

} // RayImpact
} // Impact
