#pragma once
#include "precision.hpp"
#include "Matrix4x4.hpp"
#include "Vector3.hpp"
#include "Point3.hpp"
#include "Normal3.hpp"
#include "Ray.hpp"
#include "BoundingBox.hpp"
#include "ScatteringEvent.hpp"

namespace Impact {
namespace RayImpact {

// Transformation declarations

class Transformation {

private:

	Matrix4x4 matrix; // Transformation matrix
	Matrix4x4 matrix_inverse; // Inverse of transformation matrix

public:

	Transformation();
	
	Transformation(const Matrix4x4& matrix);
	
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
	
	static Transformation worldToCamera(const Point3F& camera_position,
										const Vector3F& up_vector,
										const Point3F& look_point);

	bool operator==(const Transformation& other) const;
	bool operator!=(const Transformation& other) const;

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
};

} // RayImpact
} // Impact
