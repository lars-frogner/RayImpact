#pragma once
#include "Filter.hpp"
#include "ParameterSet.hpp"
#include "api.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// TriangleFilter declarations

class TriangleFilter : public Filter {

public:

    TriangleFilter(const Vector2F& radius);

    imp_float evaluate(const Point2F& position) const;
};

// TriangleFilter inline method definitions

inline TriangleFilter::TriangleFilter(const Vector2F& radius)
    : Filter::Filter(radius)
{}

inline imp_float TriangleFilter::evaluate(const Point2F& position) const
{
    return std::max(0.0f, radius.x - std::abs(position.x))*
           std::max(0.0f, radius.y - std::abs(position.y));
}

// TriangleFilter inline function definitions

inline Filter* createTriangleFilter(const ParameterSet& parameters)
{
    Vector2F extent = parameters.getSinglePairValue("extent", Vector2F(10.0f, 10.0f));
	
	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Reconstruction filter:"
						 "\n    %-20s%s"
						 "\n    %-20s%s px",
						 "Type:", "Triangle",
						 "Extent:", extent.toString().c_str());
	}
	
	return new TriangleFilter(extent*0.5f);
}

} // RayImpact
} // Impact
