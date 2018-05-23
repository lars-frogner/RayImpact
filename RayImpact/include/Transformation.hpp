#pragma once
#include "precision.hpp"
#include "Matrix4x4.hpp"
#include "geometry.hpp"
#include "Ray.hpp"
#include "Quaternion.hpp"
#include "BoundingBox.hpp"
#include "ScatteringEvent.hpp"
#include <ostream>
#include <sstream>
#include <string>

namespace Impact {
namespace RayImpact {

// Forward declarations
class AnimatedTransformation;

// Transformation declarations

class Transformation {

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

	std::string toString() const;
};

// Transformation inline method definitions

// Identity transformation
inline Transformation::Transformation()
    : matrix(),
      matrix_inverse()
{}

inline Transformation::Transformation(const Matrix4x4& matrix)
    : matrix(matrix),
      matrix_inverse(matrix.inverted())
{}

inline Transformation::Transformation(const Matrix4x4& matrix,
									  const Matrix4x4& matrix_inverse)
    : matrix(matrix),
      matrix_inverse(matrix_inverse)
{}

inline bool Transformation::operator==(const Transformation& other) const
{
    return matrix == other.matrix;
}

inline bool Transformation::operator!=(const Transformation& other) const
{
    return !(*this == other);
}

inline Transformation Transformation::operator*(const Transformation& other) const
{
    return Transformation(matrix*other.matrix, other.matrix_inverse*matrix_inverse);
}

inline Transformation& Transformation::operator*=(const Transformation& other)
{
    (*this) = (*this)*other;
    return *this;
}

inline bool Transformation::isIdentity() const
{
    return matrix.isIdentity();
}

inline Transformation Transformation::transposed() const
{
    return Transformation(matrix.transposed(), matrix_inverse.transposed());
}

inline Transformation Transformation::inverted() const
{
    return Transformation(matrix_inverse, matrix);
}

inline Quaternion Transformation::quaternion() const
{
    return quaternionFromMatrix(matrix);
}

inline std::string Transformation::toString() const
{
    std::ostringstream stream;
	stream << "["
           << matrix.a11 << ", "
           << matrix.a12 << ", "
           << matrix.a13 << ", "
           << matrix.a14 << "; "
           << matrix.a21 << ", "
           << matrix.a22 << ", "
           << matrix.a23 << ", "
           << matrix.a24 << "; "
           << matrix.a31 << ", "
           << matrix.a32 << ", "
           << matrix.a33 << ", "
           << matrix.a34 << "; "
           << matrix.a41 << ", "
           << matrix.a42 << ", "
           << matrix.a43 << ", "
           << matrix.a44 << "]";
    return stream.str();
}

// Transformation inline function definitions

inline std::ostream& operator<<(std::ostream& stream, const Transformation& transformation)
{
	stream << transformation.toString();
    return stream;
}


} // RayImpact
} // Impact
