#pragma once
#include "precision.hpp"
#include "geometry.hpp"

namespace Impact {
namespace RayImpact {

// Filter declarations

class Filter {

public:

	Vector2F radius; // Half the full extent of the filter in each direction
	Vector2F inverse_radius; // Reciprocal of the radius vector

	Filter(const Vector2F& radius);

	virtual imp_float evaluate(const Point2F& position) const = 0;
};

// Filter method implementations

inline Filter::Filter(const Vector2F& radius)
	: radius(radius),
	  inverse_radius(1.0f/radius.x, 1.0f/radius.y)
{}

} // RayImpact
} // Impact
