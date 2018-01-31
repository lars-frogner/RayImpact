#pragma once
#include "Filter.hpp"
#include "ParameterSet.hpp"

namespace Impact {
namespace RayImpact {

// BoxFilter declarations

class BoxFilter : public Filter {

public:

    BoxFilter(const Vector2F& radius);

    imp_float evaluate(const Point2F& position) const;
};

// BoxFilter method implementations

inline BoxFilter::BoxFilter(const Vector2F& radius)
    : Filter::Filter(radius)
{}

inline imp_float BoxFilter::evaluate(const Point2F& position) const
{
    return 1.0f;
}

// BoxFilter creation

inline Filter* createBoxFilter(const ParameterSet& parameters)
{
    Vector2F radius = parameters.getSingleVector2FValue("radius", Vector2F(5.0f, 5.0f));

    return new BoxFilter(radius);
}

} // RayImpact
} // Impact
