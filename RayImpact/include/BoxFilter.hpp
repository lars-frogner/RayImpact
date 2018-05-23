#pragma once
#include "Filter.hpp"
#include "ParameterSet.hpp"
#include "api.hpp"

namespace Impact {
namespace RayImpact {

// BoxFilter declarations

class BoxFilter : public Filter {

public:

    BoxFilter(const Vector2F& radius);

    imp_float evaluate(const Point2F& position) const;
};

// BoxFilter inline method definitions

inline BoxFilter::BoxFilter(const Vector2F& radius)
	: Filter::Filter(radius)
{}

inline imp_float BoxFilter::evaluate(const Point2F& position) const
{
    return 1.0f;
}

// BoxFilter inline function definitions

inline Filter* createBoxFilter(const ParameterSet& parameters)
{
    Vector2F extent = parameters.getSinglePairValue("extent", Vector2F(10.0f, 10.0f));
	
	if (RIMP_OPTIONS.verbosity >= IMP_CORE_VERBOSITY)
	{
		printInfoMessage("Reconstruction filter:"
						 "\n    %-20s%s"
						 "\n    %-20s%s px",
						 "Type:", "Box",
						 "Extent:", extent.toString().c_str());
	}

    return new BoxFilter(extent*0.5f);
}

} // RayImpact
} // Impact
