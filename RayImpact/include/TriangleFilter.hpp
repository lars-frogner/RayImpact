#pragma once
#include "Filter.hpp"
#include "ParameterSet.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// TriangleFilter declarations

class TriangleFilter : public Filter {

public:

	TriangleFilter(const Vector2F& radius);

	imp_float evaluate(const Point2F& position) const;
};

// TriangleFilter method implementations

inline TriangleFilter::TriangleFilter(const Vector2F& radius)
	: Filter::Filter(radius)
{}

inline imp_float TriangleFilter::evaluate(const Point2F& position) const
{
	return std::max(0.0f, radius.x - std::abs(position.x))*
		   std::max(0.0f, radius.y - std::abs(position.y));
}

// TriangleFilter creation

inline Filter* createTriangleFilter(const ParameterSet& parameters)
{
	Vector2F radius = parameters.getSingleVector2FValue("radius", Vector2F(5.0f, 5.0f));

	return new TriangleFilter(radius);
}

} // RayImpact
} // Impact
