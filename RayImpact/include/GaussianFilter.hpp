#pragma once
#include "Filter.hpp"
#include "ParameterSet.hpp"
#include "api.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// GaussianFilter declarations

class GaussianFilter : public Filter {

private:

    const imp_float sharpness;
    const imp_float edge_value_x;
    const imp_float edge_value_y;

public:

    GaussianFilter(const Vector2F& radius, imp_float sharpness);

    imp_float evaluate(const Point2F& position) const;
};

// GaussianFilter inline method definitions

inline GaussianFilter::GaussianFilter(const Vector2F& radius,
									  imp_float sharpness)
    : Filter::Filter(radius),
      sharpness(sharpness),
      edge_value_x(std::exp(-sharpness*radius.x*radius.x)),
      edge_value_y(std::exp(-sharpness*radius.y*radius.y))
{}

inline imp_float GaussianFilter::evaluate(const Point2F& position) const
{
    return std::max<imp_float>(0.0f, std::exp(-sharpness*position.x*position.x) - edge_value_x)*
           std::max<imp_float>(0.0f, std::exp(-sharpness*position.y*position.y) - edge_value_y);
}

// GaussianFilter inline function definitions

inline Filter* createGaussianFilter(const ParameterSet& parameters)
{
    Vector2F extent = parameters.getSinglePairValue("extent", Vector2F(10.0f, 10.0f));
	imp_float sharpness = parameters.getSingleFloatValue("sharpness", 1.0f);
    
	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Reconstruction filter:"
						 "\n    %-20s%s"
						 "\n    %-20s%s px"
						 "\n    %-20s%g",
						 "Type:", "Gaussian",
						 "Extent:", extent.toString().c_str(),
						 "Sharpness:", sharpness);
	}

    return new GaussianFilter(extent*0.5f, sharpness);
}

} // RayImpact
} // Impact
