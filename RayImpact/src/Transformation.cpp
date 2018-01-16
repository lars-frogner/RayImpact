#include "Transformation.hpp"
#include "error.hpp"
#include "math.hpp"
#include <cmath>

namespace Impact {
namespace RayImpact {

// Transformation method implementations
	
// Identity transformation
Transformation::Transformation()
	: matrix(),
	  matrix_inverse()
{}

Transformation::Transformation(const Matrix4x4& matrix)
	: matrix(matrix),
	  matrix_inverse(matrix.inverted())
{}

Transformation::Transformation(const Matrix4x4& matrix,
							   const Matrix4x4& matrix_inverse)
	: matrix(matrix),
	  matrix_inverse(matrix_inverse)
{}

Transformation Transformation::translation(const Vector3F& displacement)
{
	Matrix4x4 matrix(1, 0, 0, displacement.x,
					 0, 1, 0, displacement.y,
					 0, 0, 1, displacement.z,
					 0, 0, 0,			   1);
	
	Matrix4x4 matrix_inverse(1, 0, 0, -displacement.x,
							 0, 1, 0, -displacement.y,
							 0, 0, 1, -displacement.z,
							 0, 0, 0,			    1);

	return Transformation(matrix, matrix_inverse);
}

Transformation Transformation::scaling(imp_float scale_x,
									   imp_float scale_y,
									   imp_float scale_z)
{
	imp_assert(scale_x != 0 && scale_y != 0 && scale_z != 0);

	Matrix4x4 matrix(scale_x,		0,		 0, 0,
						   0, scale_y,		 0, 0,
						   0,		0, scale_z, 0,
						   0,		0,		 0, 1);
	
	Matrix4x4 matrix_inverse(1.0f/scale_x,		      0,		    0, 0,
									    0, 1.0f/scale_y,		    0, 0,
									    0,		      0, 1.0f/scale_z, 0,
									    0,		      0,		    0, 1);

	return Transformation(matrix, matrix_inverse);
}

Transformation Transformation::rotationFromXToY(imp_float angle)
{
	imp_float angle_rad = degreesToRadians(angle);

	imp_float cos_angle = std::cos(angle_rad);
    imp_float sin_angle = std::sin(angle_rad);
    
    Matrix4x4 matrix(cos_angle, -sin_angle, 0, 0,
					 sin_angle,  cos_angle, 0, 0,
							 0,			 0, 1, 0,
							 0,          0, 0, 1);

	return Transformation(matrix, matrix.transposed());
}

Transformation Transformation::rotationFromYToZ(imp_float angle)
{
	imp_float angle_rad = degreesToRadians(angle);

	imp_float cos_angle = std::cos(angle_rad);
    imp_float sin_angle = std::sin(angle_rad);
    
    Matrix4x4 matrix(1,			0,			0, 0,
					 0, cos_angle, -sin_angle, 0,
					 0, sin_angle,	cos_angle, 0,
					 0,         0,			0, 1);

	return Transformation(matrix, matrix.transposed());
}

Transformation Transformation::rotationFromZToX(imp_float angle)
{
	imp_float angle_rad = degreesToRadians(angle);

	imp_float cos_angle = std::cos(angle_rad);
    imp_float sin_angle = std::sin(angle_rad);
    
    Matrix4x4 matrix( cos_angle,  0, sin_angle, 0,
							  0,  1,		 0, 0,
					 -sin_angle,  0, cos_angle, 0,
							  0,  0,		 0, 1);

	return Transformation(matrix, matrix.transposed());
}

Transformation Transformation::rotation(const Vector3F& axis, imp_float angle)
{
	imp_float angle_rad = degreesToRadians(angle);
	
	imp_float cos_angle = std::cos(angle_rad);
    imp_float sin_angle = std::sin(angle_rad);

	const Vector3F& unit_axis = axis.normalized();

	Matrix4x4 matrix;

	matrix.a11 = (unit_axis.x*unit_axis.x) + (1 - (unit_axis.x*unit_axis.x))*cos_angle;
	matrix.a12 = (unit_axis.x*unit_axis.y)*(1 - cos_angle) - (unit_axis.z*sin_angle);
	matrix.a13 = (unit_axis.x*unit_axis.z)*(1 - cos_angle) + (unit_axis.y*sin_angle);
	
	matrix.a21 = (unit_axis.y*unit_axis.x)*(1 - cos_angle) + (unit_axis.z*sin_angle);
	matrix.a22 = (unit_axis.y*unit_axis.y) + (1 - (unit_axis.y*unit_axis.y))*cos_angle;
	matrix.a23 = (unit_axis.y*unit_axis.z)*(1 - cos_angle) - (unit_axis.x*sin_angle);

	matrix.a31 = (unit_axis.z*unit_axis.x)*(1 - cos_angle) - (unit_axis.y*sin_angle);
	matrix.a32 = (unit_axis.z*unit_axis.y)*(1 - cos_angle) + (unit_axis.x*sin_angle);
	matrix.a33 = (unit_axis.z*unit_axis.z) + (1 - (unit_axis.z*unit_axis.z))*cos_angle;

	return Transformation(matrix, matrix.transposed());
}

Transformation Transformation::worldToCamera(const Point3F& camera_position,
											 const Vector3F& up_vector,
											 const Point3F& look_point)
{
	Matrix4x4 camera_to_world_matrix;

	const Vector3F& camera_z_axis = (camera_position - look_point).normalized();
    const Vector3F& camera_y_axis = (up_vector - camera_z_axis*(camera_z_axis.dot(up_vector))).normalized();
    const Vector3F& camera_x_axis = camera_y_axis.cross(camera_z_axis);

	camera_to_world_matrix.a11 = camera_x_axis.x;
	camera_to_world_matrix.a21 = camera_x_axis.y;
	camera_to_world_matrix.a31 = camera_x_axis.z;
	
	camera_to_world_matrix.a12 = camera_y_axis.x;
	camera_to_world_matrix.a22 = camera_y_axis.y;
	camera_to_world_matrix.a32 = camera_y_axis.z;
	
	camera_to_world_matrix.a13 = camera_z_axis.x;
	camera_to_world_matrix.a23 = camera_z_axis.y;
	camera_to_world_matrix.a33 = camera_z_axis.z;
	
	camera_to_world_matrix.a14 = camera_position.x;
	camera_to_world_matrix.a24 = camera_position.y;
	camera_to_world_matrix.a34 = camera_position.z;

	return Transformation(camera_to_world_matrix.inverted(), camera_to_world_matrix);
}

// Computes the orthographic projection matrix for the given near and far plane distance
Transformation Transformation::orthographic(imp_float near_plane_distance,
											imp_float far_plane_distance)
{
	return scaling(1, 1, 1.0f/(far_plane_distance - near_plane_distance))*
		   translation(Vector3F(0, 0, -near_plane_distance));
}

// Computes the perspective projection matrix for the given field of view and near and far plane distance
Transformation Transformation::perspective(imp_float field_of_view,
										   imp_float near_plane_distance,
										   imp_float far_plane_distance)
{
	imp_float z_scale = far_plane_distance/(far_plane_distance - near_plane_distance);
	imp_float z_shift = near_plane_distance*z_scale;

	Matrix4x4 projection(1, 0,       0,       0,
						 0, 1,       0,       0,
						 0, 0, z_scale, z_shift,
						 0, 0,      -1,       0);

	// tan(FOV/2) is half the extent of the FOV at the far plane distance (z = -1) in screen space.
	// Scaling by the inverse of this maps the projected x- and y-values inside the FOV to the range [-1, 1].
	imp_float xy_scale = 1.0f/std::tan(degreesToRadians(field_of_view)*0.5f);

	return scaling(xy_scale, xy_scale, 1)*Transformation(projection);
}

bool Transformation::operator==(const Transformation& other) const
{
	return matrix == other.matrix;
}

bool Transformation::operator!=(const Transformation& other) const
{
	return !(*this == other);
}

Transformation Transformation::operator*(const Transformation& other) const
{
	return Transformation(matrix*other.matrix, other.matrix_inverse*matrix_inverse);
}

Transformation& Transformation::operator*=(const Transformation& other)
{
	(*this) = (*this)*other;
	return *this;
}

Point3F Transformation::operator()(const Point3F& point) const
{
	imp_float x = matrix.a11*point.x + matrix.a12*point.y + matrix.a13*point.z + matrix.a14;
	imp_float y = matrix.a21*point.x + matrix.a22*point.y + matrix.a23*point.z + matrix.a24;
	imp_float z = matrix.a31*point.x + matrix.a32*point.y + matrix.a33*point.z + matrix.a34;
	imp_float w = matrix.a41*point.x + matrix.a42*point.y + matrix.a43*point.z + matrix.a44;

	return (w == 1)? Point3F(x, y, z) : Point3F(x, y, z)*(1.0f/w);
}

Point3F Transformation::operator()(const Point3F& point, Vector3F* transformed_point_error) const
{
	// The error is incorrect if w != 1, i.e. for projective transformations
	imp_assert(matrix.a41*point.x + matrix.a42*point.y + matrix.a43*point.z + matrix.a44 == 1);

	*transformed_point_error = Vector3F(std::abs(matrix.a11*point.x) + std::abs(matrix.a12*point.y) + std::abs(matrix.a13*point.z) + std::abs(matrix.a14),
										std::abs(matrix.a21*point.x) + std::abs(matrix.a22*point.y) + std::abs(matrix.a23*point.z) + std::abs(matrix.a24),
										std::abs(matrix.a31*point.x) + std::abs(matrix.a32*point.y) + std::abs(matrix.a33*point.z) + std::abs(matrix.a34))
							   *errorPowerBound(3);

	return Point3F(matrix.a11*point.x + matrix.a12*point.y + matrix.a13*point.z + matrix.a14,
				   matrix.a21*point.x + matrix.a22*point.y + matrix.a23*point.z + matrix.a24,
				   matrix.a31*point.x + matrix.a32*point.y + matrix.a33*point.z + matrix.a34);
}

Point3F Transformation::operator()(const Point3F& point, const Vector3F& point_error, Vector3F* transformed_point_error) const
{
	// The error is incorrect if w != 1, i.e. for projective transformations
	imp_assert(matrix.a41*point.x + matrix.a42*point.y + matrix.a43*point.z + matrix.a44 == 1);

	*transformed_point_error = Vector3F(std::abs(matrix.a11*point.x) + std::abs(matrix.a12*point.y) + std::abs(matrix.a13*point.z) + std::abs(matrix.a14),
										std::abs(matrix.a21*point.x) + std::abs(matrix.a22*point.y) + std::abs(matrix.a23*point.z) + std::abs(matrix.a24),
										std::abs(matrix.a31*point.x) + std::abs(matrix.a32*point.y) + std::abs(matrix.a33*point.z) + std::abs(matrix.a34))
							   *errorPowerBound(3) + 
							   Vector3F(std::abs(matrix.a11)*point_error.x + std::abs(matrix.a12)*point_error.y + std::abs(matrix.a13)*point_error.z + std::abs(matrix.a14),
										std::abs(matrix.a21)*point_error.x + std::abs(matrix.a22)*point_error.y + std::abs(matrix.a23)*point_error.z + std::abs(matrix.a24),
										std::abs(matrix.a31)*point_error.x + std::abs(matrix.a32)*point_error.y + std::abs(matrix.a33)*point_error.z + std::abs(matrix.a34))
							   *(errorPowerBound(3) + 1);
	
	return Point3F(matrix.a11*point.x + matrix.a12*point.y + matrix.a13*point.z + matrix.a14,
				   matrix.a21*point.x + matrix.a22*point.y + matrix.a23*point.z + matrix.a24,
				   matrix.a31*point.x + matrix.a32*point.y + matrix.a33*point.z + matrix.a34);
}

Vector3F Transformation::operator()(const Vector3F& vector) const
{
	return Vector3F(matrix.a11*vector.x + matrix.a12*vector.y + matrix.a13*vector.z,
					matrix.a21*vector.x + matrix.a22*vector.y + matrix.a23*vector.z,
					matrix.a31*vector.x + matrix.a32*vector.y + matrix.a33*vector.z);
}

Vector3F Transformation::operator()(const Vector3F& vector, Vector3F* transformed_vector_error) const
{
	*transformed_vector_error = Vector3F(std::abs(matrix.a11*vector.x) + std::abs(matrix.a12*vector.y) + std::abs(matrix.a13*vector.z),
										 std::abs(matrix.a21*vector.x) + std::abs(matrix.a22*vector.y) + std::abs(matrix.a23*vector.z),
										 std::abs(matrix.a31*vector.x) + std::abs(matrix.a32*vector.y) + std::abs(matrix.a33*vector.z))
								*errorPowerBound(3);

	return Vector3F(matrix.a11*vector.x + matrix.a12*vector.y + matrix.a13*vector.z,
					matrix.a21*vector.x + matrix.a22*vector.y + matrix.a23*vector.z,
					matrix.a31*vector.x + matrix.a32*vector.y + matrix.a33*vector.z);
}

Vector3F Transformation::operator()(const Vector3F& vector, const Vector3F& vector_error, Vector3F* transformed_vector_error) const
{
	*transformed_vector_error = Vector3F(std::abs(matrix.a11*vector.x) + std::abs(matrix.a12*vector.y) + std::abs(matrix.a13*vector.z),
										 std::abs(matrix.a21*vector.x) + std::abs(matrix.a22*vector.y) + std::abs(matrix.a23*vector.z),
										 std::abs(matrix.a31*vector.x) + std::abs(matrix.a32*vector.y) + std::abs(matrix.a33*vector.z))
								*errorPowerBound(3) +
								Vector3F(std::abs(matrix.a11)*vector_error.x + std::abs(matrix.a12)*vector_error.y + std::abs(matrix.a13)*vector_error.z,
			 							 std::abs(matrix.a21)*vector_error.x + std::abs(matrix.a22)*vector_error.y + std::abs(matrix.a23)*vector_error.z,
										 std::abs(matrix.a31)*vector_error.x + std::abs(matrix.a32)*vector_error.y + std::abs(matrix.a33)*vector_error.z)
								*(errorPowerBound(3) + 1);

	return Vector3F(matrix.a11*vector.x + matrix.a12*vector.y + matrix.a13*vector.z,
					matrix.a21*vector.x + matrix.a22*vector.y + matrix.a23*vector.z,
					matrix.a31*vector.x + matrix.a32*vector.y + matrix.a33*vector.z);
}

Normal3F Transformation::operator()(const Normal3F& normal) const
{
	return Normal3F(matrix_inverse.a11*normal.x + matrix_inverse.a21*normal.y + matrix_inverse.a31*normal.z,
					matrix_inverse.a12*normal.x + matrix_inverse.a22*normal.y + matrix_inverse.a32*normal.z,
					matrix_inverse.a13*normal.x + matrix_inverse.a23*normal.y + matrix_inverse.a33*normal.z);
}

Ray Transformation::operator()(const Ray& ray) const
{
	const Transformation& transformation = *this;

	Vector3F origin_error;

	Point3F& origin = transformation(ray.origin, &origin_error);
	const Vector3F& direction = transformation(ray.direction);

	// Offset the transformed origin in the direction of the transformed ray
	// by an amount equal to the size of the origin error

	imp_float direction_length_squared = direction.squaredLength();
	imp_float max_distance = ray.max_distance;

	if (direction_length_squared > 0)
	{
		imp_float offset_distance = origin_error.dot(abs(direction))/direction_length_squared;

		origin += direction*offset_distance;
		max_distance -= offset_distance;
	}

	return Ray(origin, direction, max_distance, ray.time, ray.medium);
}

Ray Transformation::operator()(const Ray& ray, Vector3F* transformed_origin_error, Vector3F* transformed_direction_error) const
{
	const Transformation& transformation = *this;

	Point3F& origin = transformation(ray.origin, transformed_origin_error);
	const Vector3F& direction = transformation(ray.direction, transformed_direction_error);

	// Offset the transformed origin in the direction of the transformed ray
	// by an amount equal to the size of the origin error

	imp_float direction_length_squared = direction.squaredLength();
	imp_float max_distance = ray.max_distance;

	if (direction_length_squared > 0)
	{
		imp_float offset_distance = transformed_origin_error->dot(abs(direction))/direction_length_squared;

		origin += direction*offset_distance;
		max_distance -= offset_distance;
	}

	return Ray(origin, direction, max_distance, ray.time, ray.medium);
}

RayWithOffsets Transformation::operator()(const RayWithOffsets& ray) const
{
	const Transformation& transformation = *this;

	const Ray& transformed_ray = transformation(Ray(ray.origin, ray.direction, ray.max_distance, ray.time, ray.medium));

	RayWithOffsets transformed_ray_with_offsets(transformed_ray);

	if (ray.has_offsets)
	{
		transformed_ray_with_offsets.x_offset_ray_origin = transformation(ray.x_offset_ray_origin);
		transformed_ray_with_offsets.x_offset_ray_direction = transformation(ray.x_offset_ray_direction);

		transformed_ray_with_offsets.y_offset_ray_origin = transformation(ray.y_offset_ray_origin);
		transformed_ray_with_offsets.y_offset_ray_direction = transformation(ray.y_offset_ray_direction);

		transformed_ray_with_offsets.has_offsets = true;
	}

	return transformed_ray_with_offsets;
}

BoundingBoxF Transformation::operator()(const BoundingBoxF& box) const
{
	const Vector3F& diagonal = box.diagonal();

	Vector3F transformed_width_vector (matrix.a11*diagonal.x, matrix.a21*diagonal.x, matrix.a31*diagonal.x);
	Vector3F transformed_height_vector(matrix.a12*diagonal.y, matrix.a22*diagonal.y, matrix.a22*diagonal.y);
	Vector3F transformed_depth_vector (matrix.a13*diagonal.z, matrix.a23*diagonal.z, matrix.a33*diagonal.z);
	
	const Point3F& corner_1 = (*this)(box.lower_corner);
	const Point3F& corner_2 = corner_1 + transformed_width_vector;
	const Point3F& corner_3 = corner_1 + transformed_height_vector;
	const Point3F& corner_4 = corner_2 + transformed_height_vector;
	const Point3F& corner_5 = corner_1 + transformed_depth_vector;
	const Point3F& corner_6 = corner_2 + transformed_depth_vector;
	const Point3F& corner_7 = corner_3 + transformed_depth_vector;
	const Point3F& corner_8 = corner_4 + transformed_depth_vector;

	BoundingBoxF transformed_box;

	transformed_box.enclose(corner_1);
	transformed_box.enclose(corner_2);
	transformed_box.enclose(corner_3);
	transformed_box.enclose(corner_4);
	transformed_box.enclose(corner_5);
	transformed_box.enclose(corner_6);
	transformed_box.enclose(corner_7);
	transformed_box.enclose(corner_8);

	return transformed_box;
}

SurfaceScatteringEvent Transformation::operator()(const SurfaceScatteringEvent& scattering_event) const
{
	SurfaceScatteringEvent transformed_event(scattering_event);
	
	const Transformation& transformation = *this;

	transformed_event.position = transformation(scattering_event.position, scattering_event.position_error, &(transformed_event.position_error));

	transformed_event.outgoing_direction = transformation(scattering_event.outgoing_direction).normalized();
	transformed_event.surface_normal = transformation(scattering_event.surface_normal).normalized();

	transformed_event.position_u_deriv = transformation(scattering_event.position_u_deriv);
	transformed_event.position_v_deriv = transformation(scattering_event.position_v_deriv);
	transformed_event.normal_u_deriv = transformation(scattering_event.normal_u_deriv);
	transformed_event.normal_v_deriv = transformation(scattering_event.normal_v_deriv);
	
	transformed_event.shading.surface_normal = transformation(scattering_event.shading.surface_normal).normalized();
	
	transformed_event.shading.position_u_deriv = transformation(scattering_event.shading.position_u_deriv);
	transformed_event.shading.position_v_deriv = transformation(scattering_event.shading.position_v_deriv);
	transformed_event.shading.normal_u_deriv = transformation(scattering_event.shading.normal_u_deriv);
	transformed_event.shading.normal_v_deriv = transformation(scattering_event.shading.normal_v_deriv);

	transformed_event.shading.surface_normal.flipToSameHemisphereAs(transformed_event.surface_normal);

	return transformed_event;
}

bool Transformation::isIdentity() const
{
	return matrix.isIdentity();
}

bool Transformation::hasScaling() const
{
	imp_float x_scale = Vector3F(matrix.a11, matrix.a21, matrix.a31).squaredLength();
	imp_float y_scale = Vector3F(matrix.a12, matrix.a22, matrix.a32).squaredLength();
	imp_float z_scale = Vector3F(matrix.a13, matrix.a23, matrix.a33).squaredLength();

	return (x_scale < 0.999f || x_scale > 1.001f) ||
		   (y_scale < 0.999f || y_scale > 1.001f) ||
		   (z_scale < 0.999f || z_scale > 1.001f);
}

bool Transformation::swapsHandedness() const
{
	// Determinant of upper-left 3x3 submatrix
	imp_float determinant = matrix.a11*(matrix.a22*matrix.a33 - matrix.a23*matrix.a32) -
							matrix.a12*(matrix.a21*matrix.a33 - matrix.a23*matrix.a31) +
							matrix.a13*(matrix.a21*matrix.a32 - matrix.a22*matrix.a31);

	return determinant < 0;
}

Transformation Transformation::transposed() const
{
	return Transformation(matrix.transposed(), matrix_inverse.transposed());
}

Transformation Transformation::inverted() const
{
	return Transformation(matrix_inverse, matrix);
}

} // RayImpact
} // Impact
