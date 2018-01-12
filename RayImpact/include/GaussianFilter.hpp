#pragma once
#include "Filter.hpp"
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

	GaussianFilter(const Vector2F& radius,
				   imp_float sharpness);

	imp_float evaluate(const Point2F& position) const;
};

// GaussianFilter method implementations

GaussianFilter::GaussianFilter(const Vector2F& radius,
							   imp_float sharpness)
	: Filter::Filter(radius),
	  sharpness(sharpness),
	  edge_value_x(std::exp(-sharpness*radius.x*radius.x)),
	  edge_value_y(std::exp(-sharpness*radius.y*radius.y))
{}

imp_float GaussianFilter::evaluate(const Point2F& position) const
{
	return std::max(0.0f, std::exp(-sharpness*position.x*position.x) - edge_value_x)*
		   std::max(0.0f, std::exp(-sharpness*position.y*position.y) - edge_value_y);
}

} // RayImpact
} // Impact
