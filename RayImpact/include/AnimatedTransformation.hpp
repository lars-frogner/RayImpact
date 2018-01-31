#pragma once
#include "precision.hpp"
#include "Matrix4x4.hpp"
#include "geometry.hpp"
#include "Quaternion.hpp"
#include "Transformation.hpp"
#include "Ray.hpp"
#include "BoundingBox.hpp"

namespace Impact {
namespace RayImpact {

// AnimatedTransformation declarations

class AnimatedTransformation {

private:

    const Transformation* initial_transformation; // The transformation at the start time
    const Transformation* final_transformation; // The transformation at the end time
    const imp_float start_time; // Point in time of the initial transformation state
    const imp_float end_time; // Point in time of the final transformation state
    const bool is_animated; // Whether the initial and final transformations differ
    Vector3F translation_components[2]; // Translation components of the initial and final transformations
    Quaternion rotation_components[2]; // Rotation components of the initial and final transformations
    Matrix4x4 scaling_components[2]; // Scaling components of the initial and final transformations
    bool has_rotation; // Whether there is a rotational difference between the initial and final transformation

    static void decompose(const Matrix4x4& matrix,
                          Vector3F* translation_component,
                          Quaternion* rotation_component,
                          Matrix4x4* scaling_component);

    BoundingBoxF boundedMotionOfPoint(const Point3F& point) const;

public:

    AnimatedTransformation(const Transformation* initial_transformation,
                           const Transformation* final_transformation,
                           imp_float start_time, imp_float end_time);

    void computeInterpolatedTransformation(Transformation* interpolated_transformation,
                                           imp_float time) const;

    Point3F operator()(const Point3F& point, imp_float time) const;
    
    Vector3F operator()(const Vector3F& vector, imp_float time) const;

    Ray operator()(const Ray& ray) const;

    RayWithOffsets operator()(const RayWithOffsets& ray) const;

    BoundingBoxF encompassMotionInBoundingBox(const BoundingBoxF& initial_bounds) const;
};

} // RayImpact
} // Impact
