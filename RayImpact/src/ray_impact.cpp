#include "error.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include "BoundingRectangle.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "BoxFilter.hpp"
#include "Sensor.hpp"
#include "PerspectiveCamera.hpp"
#include "OrthographicCamera.hpp"
#include "StratifiedSampler.hpp"
#include "Sphere.hpp"
#include "Model.hpp"
#include "ScatteringEvent.hpp"
#include <memory>
#include "image_util.hpp"

using namespace Impact;
using namespace RayImpact;

int main(int argc, char *argv[])
{
	std::unique_ptr<Filter> filter(new BoxFilter(Vector2F(1.0f, 1.0f)));

	Sensor sensor(Vector2I(500, 500),
				  BoundingRectangleF(Point2F(0.0f, 0.0f), Point2F(1.0f, 1.0f)),
				  std::move(filter),
				  10000.0f,
				  "test.pfm",
				  1.0f);

	std::unique_ptr<SensorRegion> sensor_region = sensor.sensorRegion(sensor.samplingBounds());

	Transformation camera_to_world = Transformation::worldToCamera(Point3F(0, 0, 0), Vector3F(0, 1, 0), Point3F(0, 0, -1)).inverted();

	PerspectiveCamera camera(camera_to_world,
							  sensor.physicalExtent(),
							  0.0f,
							  0.01f,
							  0.0f,
							  5.0f,
							  45.0f,
							  &sensor,
							  nullptr);

	Transformation sphere_to_world = Transformation::translation(Vector3F(0, 0, -6.0f));
	Transformation world_to_sphere = sphere_to_world.inverted();

	std::shared_ptr<Shape> sphere(new Sphere(&sphere_to_world, &world_to_sphere, false, 3.0f, -3.0f, 3.0f, 220.0f));
	
	StratifiedSampler sampler(1, 1, false, 5);

	Ray eye_ray;
	imp_float distance;
	SurfaceScatteringEvent scattering_event;
	Spectrum spectrum;

	for (int j = 0; j < sensor.full_resolution.y; j++) {
		for (int i = 0; i < sensor.full_resolution.x; i++)
		{
			Point2I pixel_position(i, j);

			sampler.setPixel(pixel_position);

			const CameraSample& camera_sample = sampler.generateCameraSample(pixel_position);

			camera.generateRay(camera_sample, &eye_ray);

			if (sphere->intersect(eye_ray, &distance, &scattering_event))
			{
				spectrum = distance;
			}
			else
			{
				spectrum = 0.0f;
			}
			
			sensor_region->addSample(camera_sample.sensor_point, spectrum);
		}
	}

	sensor.mergeSensorRegion(std::move(sensor_region));

	sensor.writeImage(1.0f/6.0f);
}
