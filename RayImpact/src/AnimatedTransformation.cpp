#include "AnimatedTransformation.hpp"
#include "error.hpp"
#include <algorithm>

namespace Impact {
namespace RayImpact {

// AnimatedTransformation method implementations

AnimatedTransformation::AnimatedTransformation(const Transformation* initial_transformation,
                                               const Transformation* final_transformation,
                                               imp_float start_time, imp_float end_time)
    : initial_transformation(initial_transformation),
      final_transformation(final_transformation),
      start_time(start_time), end_time(end_time),
      is_animated(*initial_transformation != *final_transformation)
{
    imp_assert(end_time >= start_time);

    decompose(initial_transformation->matrix, &translation_components[0], &rotation_components[0], &scaling_components[0]);
    decompose(final_transformation->matrix, &translation_components[1], &rotation_components[1], &scaling_components[1]);

    // Flip one of the quaternions of they are not in the same hemisphere,
    // so that the shortest rotation path is used for interpolation
    if (rotation_components[0].dot(rotation_components[1]) < 0)
        rotation_components[1] = -rotation_components[1];

    has_rotation = rotation_components[0].dot(rotation_components[1]) < 0.9995f;
}

void AnimatedTransformation::decompose(const Matrix4x4& matrix,
                                       Vector3F* translation_component,
                                       Quaternion* rotation_component,
                                       Matrix4x4* scaling_component)
{
    // Find translation component
    translation_component->x = matrix.a14;
    translation_component->y = matrix.a24;
    translation_component->z = matrix.a34;

    // Create new version of the matrix without the translation component
    Matrix4x4 matrix_without_translation(matrix.a11, matrix.a12, matrix.a13, 0,
                                         matrix.a21, matrix.a22, matrix.a23, 0,
                                         matrix.a31, matrix.a32, matrix.a33, 0,
                                                  0,          0,          0, 1);

    // Compute rotation part of the new matrix using polar decomposition
    // (iterative scheme where a better approximation is obtained by the
    // average of the current approximation and its inverse transpose)

    Matrix4x4 rotation_matrix = matrix_without_translation;
    Matrix4x4 averaged_matrix;

    imp_float norm;
    unsigned int count = 0;

    do
    {
        const Matrix4x4& inverse_transpose_matrix = rotation_matrix.transposed().inverted();

        averaged_matrix.a11 = 0.5f*(rotation_matrix.a11 + inverse_transpose_matrix.a11);
        averaged_matrix.a12 = 0.5f*(rotation_matrix.a12 + inverse_transpose_matrix.a12);
        averaged_matrix.a13 = 0.5f*(rotation_matrix.a13 + inverse_transpose_matrix.a13);
        averaged_matrix.a14 = 0.5f*(rotation_matrix.a14 + inverse_transpose_matrix.a14);
        averaged_matrix.a21 = 0.5f*(rotation_matrix.a21 + inverse_transpose_matrix.a21);
        averaged_matrix.a22 = 0.5f*(rotation_matrix.a22 + inverse_transpose_matrix.a22);
        averaged_matrix.a23 = 0.5f*(rotation_matrix.a23 + inverse_transpose_matrix.a23);
        averaged_matrix.a24 = 0.5f*(rotation_matrix.a24 + inverse_transpose_matrix.a24);
        averaged_matrix.a31 = 0.5f*(rotation_matrix.a31 + inverse_transpose_matrix.a31);
        averaged_matrix.a32 = 0.5f*(rotation_matrix.a32 + inverse_transpose_matrix.a32);
        averaged_matrix.a33 = 0.5f*(rotation_matrix.a33 + inverse_transpose_matrix.a33);
        averaged_matrix.a34 = 0.5f*(rotation_matrix.a34 + inverse_transpose_matrix.a34);
        averaged_matrix.a41 = 0.5f*(rotation_matrix.a41 + inverse_transpose_matrix.a41);
        averaged_matrix.a42 = 0.5f*(rotation_matrix.a42 + inverse_transpose_matrix.a42);
        averaged_matrix.a43 = 0.5f*(rotation_matrix.a43 + inverse_transpose_matrix.a43);
        averaged_matrix.a44 = 0.5f*(rotation_matrix.a44 + inverse_transpose_matrix.a44);

        imp_float norm_1 = std::abs(rotation_matrix.a11 - averaged_matrix.a11) +
                           std::abs(rotation_matrix.a12 - averaged_matrix.a12) +
                           std::abs(rotation_matrix.a13 - averaged_matrix.a13);

        imp_float norm_2 = std::abs(rotation_matrix.a21 - averaged_matrix.a21) +
                           std::abs(rotation_matrix.a22 - averaged_matrix.a22) +
                           std::abs(rotation_matrix.a23 - averaged_matrix.a23);

        imp_float norm_3 = std::abs(rotation_matrix.a31 - averaged_matrix.a31) +
                           std::abs(rotation_matrix.a32 - averaged_matrix.a32) +
                           std::abs(rotation_matrix.a33 - averaged_matrix.a33);

        norm = std::max({norm_1, norm_2, norm_3});

        rotation_matrix = averaged_matrix;

    } while (++count < 100 && norm > 0.0001f);

    *rotation_component = Transformation::quaternionFromMatrix(rotation_matrix);

    // Compute scaling component as the non-translation matrix without the rotation component
    *scaling_component = rotation_matrix.inverted()*matrix_without_translation;
}

void AnimatedTransformation::computeInterpolatedTransformation(Transformation* interpolated_transformation,
                                                               imp_float time) const
{
    if (!is_animated || time <= start_time)
    {
        *interpolated_transformation = *initial_transformation;
        return;
    }
    if (time >= end_time)
    {
        *interpolated_transformation = *final_transformation;
        return;
    }

    imp_float weight = (time - start_time)/(end_time - start_time);

    const Vector3F& translation_component = translation_components[0]*(1 - weight) + translation_components[1]*weight;

    const Quaternion& rotation_component = slerp(rotation_components[0], rotation_components[1], weight);

    Matrix4x4 scaling_component;

    scaling_component.a11 = ::Impact::lerp(scaling_components[0].a11, scaling_components[1].a11, weight);
    scaling_component.a12 = ::Impact::lerp(scaling_components[0].a12, scaling_components[1].a12, weight);
    scaling_component.a13 = ::Impact::lerp(scaling_components[0].a13, scaling_components[1].a13, weight);

    scaling_component.a21 = ::Impact::lerp(scaling_components[0].a21, scaling_components[1].a21, weight);
    scaling_component.a22 = ::Impact::lerp(scaling_components[0].a22, scaling_components[1].a22, weight);
    scaling_component.a23 = ::Impact::lerp(scaling_components[0].a23, scaling_components[1].a23, weight);

    scaling_component.a31 = ::Impact::lerp(scaling_components[0].a31, scaling_components[1].a31, weight);
    scaling_component.a32 = ::Impact::lerp(scaling_components[0].a32, scaling_components[1].a32, weight);
    scaling_component.a33 = ::Impact::lerp(scaling_components[0].a33, scaling_components[1].a33, weight);

    *interpolated_transformation = Transformation::translation(translation_component)*
                                   Transformation::rotation(rotation_component)*
                                   Transformation(scaling_component);
}

Point3F AnimatedTransformation::operator()(const Point3F& point, imp_float time) const
{
    if (!is_animated || time <= start_time)
        return (*initial_transformation)(point);

    if (time >= end_time)
        return (*final_transformation)(point);

    Transformation transformation;
    computeInterpolatedTransformation(&transformation, time);

    return transformation(point);
}

Vector3F AnimatedTransformation::operator()(const Vector3F& vector, imp_float time) const
{
    if (!is_animated || time <= start_time)
        return (*initial_transformation)(vector);

    if (time >= end_time)
        return (*final_transformation)(vector);

    Transformation transformation;
    computeInterpolatedTransformation(&transformation, time);

    return transformation(vector);
}

Ray AnimatedTransformation::operator()(const Ray& ray) const
{
    if (!is_animated || ray.time <= start_time)
        return (*initial_transformation)(ray);

    if (ray.time >= end_time)
        return (*final_transformation)(ray);

    Transformation transformation;
    computeInterpolatedTransformation(&transformation, ray.time);

    return transformation(ray);
}

RayWithOffsets AnimatedTransformation::operator()(const RayWithOffsets& ray) const
{
    if (!is_animated || ray.time <= start_time)
        return (*initial_transformation)(ray);

    if (ray.time >= end_time)
        return (*final_transformation)(ray);

    Transformation transformation;
    computeInterpolatedTransformation(&transformation, ray.time);

    return transformation(ray);
}

BoundingBoxF AnimatedTransformation::boundedMotionOfPoint(const Point3F& point) const
{
    printSevereMessage("method \"boundedMotionOfPoint()\" of AnimatedTransformation has not been implemented");
    return BoundingBoxF();
}

BoundingBoxF AnimatedTransformation::encompassMotionInBoundingBox(const BoundingBoxF& initial_bounds) const
{
    if (!is_animated)
        return (*initial_transformation)(initial_bounds);

    if (!has_rotation)
        return unionOf((*initial_transformation)(initial_bounds), (*final_transformation)(initial_bounds));

    BoundingBoxF bounds;

    for (unsigned int corner_idx = 0; corner_idx < 8; corner_idx++)
    {
        bounds = unionOf(bounds, boundedMotionOfPoint(initial_bounds.corner(corner_idx)));
    }

    return bounds;
}

} // RayImpact
} // Impact
