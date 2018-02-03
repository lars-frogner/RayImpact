#pragma once
#include "Filter.hpp"
#include "ParameterSet.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// GaussianFilter implementation

class GaussianFilter : public Filter {

private:

    const imp_float sharpness;
    const imp_float edge_value_x;
    const imp_float edge_value_y;

public:

    GaussianFilter(const Vector2F& radius,
                   imp_float sharpness)
        : Filter::Filter(radius),
          sharpness(sharpness),
          edge_value_x(std::exp(-sharpness*radius.x*radius.x)),
          edge_value_y(std::exp(-sharpness*radius.y*radius.y))
    {}

    imp_float evaluate(const Point2F& position) const
    {
        return std::max<imp_float>(0.0f, std::exp(-sharpness*position.x*position.x) - edge_value_x)*
               std::max<imp_float>(0.0f, std::exp(-sharpness*position.y*position.y) - edge_value_y);
    }
};

// GaussianFilter creation

inline Filter* createGaussianFilter(const ParameterSet& parameters)
{
    Vector2F radius = parameters.getSingleVector2FValue("radius", Vector2F(5.0f, 5.0f));
    imp_float sharpness = parameters.getSingleFloatValue("sharpness", 1.0f);

    return new GaussianFilter(radius, sharpness);
}

} // RayImpact
} // Impact
