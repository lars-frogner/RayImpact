#pragma once
#include "precision.hpp"
#include "Camera.hpp"

namespace Impact {
namespace RayImpact {

// PerspectiveCamera declarations

class PerspectiveCamera : public ProjectiveCamera {

private:

	Vector3F horizontal_pixel_offset; // The horizontal offset between two pixels in camera space
	Vector3F vertical_pixel_offset; // The vertical offset between two pixels in camera space

public:

	PerspectiveCamera(const AnimatedTransformation& camera_to_world,
					   const BoundingRectangleF& screen_window,
				       imp_float shutter_opening_time,
				       imp_float shutter_closing_time,
				       imp_float lens_radius,
				       imp_float focal_distance,
					   imp_float field_of_view,
				       Sensor* sensor,
				       const Medium* medium);

	imp_float generateRay(const CameraSample& sample,
						  Ray* ray) const;
	
	imp_float generateRayWithOffsets(const CameraSample& sample,
									 RayWithOffsets* ray) const;
};

} // RayImpact
} // Impact
