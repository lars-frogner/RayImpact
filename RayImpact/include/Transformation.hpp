#pragma once
#include "precision.hpp"
#include "Matrix4x4.hpp"
#include "geometry.hpp"
#include "Ray.hpp"
#include "Quaternion.hpp"
#include "BoundingBox.hpp"
#include "ScatteringEvent.hpp"
#include <ostream>

namespace Impact {
namespace RayImpact {

// Forward declarations
class AnimatedTransformation;

// Transformation declarations

class Transformation {

friend std::ostream& operator<<(std::ostream& stream, const Transformation& transformation);

friend AnimatedTransformation;

protected:

    Matrix4x4 matrix; // Transformation matrix
    Matrix4x4 matrix_inverse; // Inverse of transformation matrix

    static Quaternion quaternionFromMatrix(const Matrix4x4& matrix);

public:

    Transformation();
    
    explicit Transformation(const Matrix4x4& matrix);
    
    Transformation(const Matrix4x4& matrix,
                   const Matrix4x4& matrix_inverse);

    static Transformation translation(const Vector3F& displacement);

    static Transformation scaling(imp_float scale_x,
                                  imp_float scale_y,
                                  imp_float scale_z);
    
    static Transformation rotationFromXToY(imp_float angle);
    static Transformation rotationFromYToZ(imp_float angle);
    static Transformation rotationFromZToX(imp_float angle);
    
    static Transformation rotation(const Vector3F& axis, imp_float angle);

    static Transformation rotation(const Quaternion& quaternion);
    
    static Transformation worldToCamera(const Point3F& camera_position,
                                        const Vector3F& up_vector,
                                        const Point3F& look_point);

    static Transformation orthographic(imp_float near_plane_distance,
                                       imp_float far_plane_distance);

    static Transformation perspective(imp_float field_of_view,
                                      imp_float near_plane_distance,
                                      imp_float far_plane_distance);

    bool operator==(const Transformation& other) const;
    bool operator!=(const Transformation& other) const;
    
    bool operator<(const Transformation& other) const;

    Transformation operator*(const Transformation& other) const;
    Transformation& operator*=(const Transformation& other);

    Point3F operator()(const Point3F& point) const;
    Point3F operator()(const Point3F& point, Vector3F* transformed_point_error) const;
    Point3F operator()(const Point3F& point, const Vector3F& point_error, Vector3F* transformed_point_error) const;

    Vector3F operator()(const Vector3F& vector) const;
    Vector3F operator()(const Vector3F& vector, Vector3F* transformed_vector_error) const;
    Vector3F operator()(const Vector3F& vector, const Vector3F& vector_error, Vector3F* transformed_vector_error) const;

    Normal3F operator()(const Normal3F& normal) const;

    Ray operator()(const Ray& normal) const;
    Ray operator()(const Ray& ray, Vector3F* transformed_origin_error, Vector3F* transformed_direction_error) const;
    RayWithOffsets operator()(const RayWithOffsets& normal) const;

    BoundingBoxF operator()(const BoundingBoxF& box) const;
    SurfaceScatteringEvent operator()(const SurfaceScatteringEvent& scattering_event) const;

    bool isIdentity() const;

    bool hasScaling() const;

    bool swapsHandedness() const;
    
    Transformation transposed() const;

    Transformation inverted() const;

    Quaternion quaternion() const;
};

} // RayImpact
} // Impact
