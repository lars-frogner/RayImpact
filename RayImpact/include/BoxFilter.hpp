#pragma once
#include "Filter.hpp"

namespace Impact {
namespace RayImpact {

// BoxFilter declarations

class BoxFilter : public Filter {

public:

	BoxFilter(const Vector2F& radius);

	imp_float evaluate(const Point2F& position) const;
};

// BoxFilter method implementations

BoxFilter::BoxFilter(const Vector2F& radius)
	: Filter::Filter(radius)
{}

imp_float BoxFilter::evaluate(const Point2F& position) const
{
	return 1.0f;
}

} // RayImpact
} // Impact
