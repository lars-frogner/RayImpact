#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include <ostream>

namespace Impact {
namespace RayImpact {

// Ray declarations

class Ray {

public:

	Point3F origin; // Ray origin
	Vector3F direction; // Ray direction (generally not normalized)
	mutable imp_float max_distance; // Distance from ray origin to ray endpoint
	imp_float time; // Point in time associated with the ray
	const Medium* medium; // Medium associated with ray

	Ray();

	Ray(const Point3F& origin,
	    const Vector3F& direction,
		imp_float max_distance = IMP_INFINITY,
		imp_float time = 0,
		const Medium* medium = nullptr);

	Point3F operator()(imp_float distance) const;
};

// RayWithOffsets declarations

class RayWithOffsets : public Ray {

public:

	bool has_offsets;

	Point3F x_offset_ray_origin; // Origin of x-offset ray
	Vector3F x_offset_ray_direction; // Direction of x-offset ray

	Point3F y_offset_ray_origin; // Origin of y-offset ray
	Vector3F y_offset_ray_direction; // Direction of y-offset ray

	RayWithOffsets();
	
	RayWithOffsets(const Point3F& origin,
				   const Vector3F& direction,
				   imp_float max_distance = IMP_INFINITY,
				   imp_float time = 0,
				   const Medium* medium = nullptr);

	RayWithOffsets(const Ray& ray);

	void scaleOffsets(imp_float scale);
};

// Printing functions
std::ostream& operator<<(std::ostream& stream, const Ray& ray);
std::ostream& operator<<(std::ostream& stream, const RayWithOffsets& ray);

} // RayImpact
} // Impact
