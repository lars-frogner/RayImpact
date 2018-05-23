#pragma once
#include "precision.hpp"
#include "error.hpp"
#include "RegionAllocator.hpp"
#include "BoundingBox.hpp"
#include "Ray.hpp"
#include "Transformation.hpp"
#include "AnimatedTransformation.hpp"
#include "ScatteringEvent.hpp"
#include "Shape.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// Model declarations

class Model {

public:

    virtual BoundingBoxF worldSpaceBoundingBox() const = 0;

    virtual bool intersect(const Ray& ray,
                           SurfaceScatteringEvent* scattering_event) const = 0;

    virtual bool hasIntersection(const Ray& ray) const = 0;

    virtual const AreaLight* getAreaLight() const = 0;

    virtual const Material* getMaterial() const = 0;

    virtual void generateBSDF(SurfaceScatteringEvent* scattering_event,
                              RegionAllocator& allocator,
                              TransportMode transport_mode,
                              bool allow_multiple_scattering_types) const = 0;
};

// GeometricModel declarations

class GeometricModel : public Model {

private:

    std::shared_ptr<Shape> shape; // The shape associated with the model
    std::shared_ptr<Material> material; // The material associated with the model
    std::shared_ptr<AreaLight> area_light; // The area light (if any) associated with the model
    MediumInterface medium_interface; // The interface between the media on the inside and the outside of the model

public:

    GeometricModel(const std::shared_ptr<Shape>& shape,
                   const std::shared_ptr<Material>& material,
                   const std::shared_ptr<AreaLight>& area_light,
                   const MediumInterface& medium_interface);

    BoundingBoxF worldSpaceBoundingBox() const;

    bool intersect(const Ray& ray,
                   SurfaceScatteringEvent* scattering_event) const;

    bool hasIntersection(const Ray& ray) const;

    const AreaLight* getAreaLight() const;

    const Material* getMaterial() const;

    void generateBSDF(SurfaceScatteringEvent* scattering_event,
                      RegionAllocator& allocator,
                      TransportMode transport_mode,
                      bool allow_multiple_scattering_types) const;
};

// TransformedModel declarations

class TransformedModel : public Model {

private:

    std::shared_ptr<Model> model; // The underlying model for the transformed model
    const AnimatedTransformation model_to_world; // The transformation from the shape's notion of world space to actual world space

public:

    TransformedModel(const std::shared_ptr<Model>& model,
                     const AnimatedTransformation& model_to_world);

    BoundingBoxF worldSpaceBoundingBox() const;

    bool intersect(const Ray& ray,
                   SurfaceScatteringEvent* scattering_event) const;

    bool hasIntersection(const Ray& ray) const;

    const AreaLight* getAreaLight() const;

    const Material* getMaterial() const;

    void generateBSDF(SurfaceScatteringEvent* scattering_event,
                      RegionAllocator& allocator,
                      TransportMode transport_mode,
                      bool allow_multiple_scattering_types) const;
};

// AccelerationStructure declarations

class AccelerationStructure : public Model {

public:

    const AreaLight* getAreaLight() const;

    const Material* getMaterial() const;

    void generateBSDF(SurfaceScatteringEvent* scattering_event,
                      RegionAllocator& allocator,
                      TransportMode transport_mode,
                      bool allow_multiple_scattering_types) const;
};

// GeometricModel inline method definitions

inline GeometricModel::GeometricModel(const std::shared_ptr<Shape>& shape,
									  const std::shared_ptr<Material>& material,
									  const std::shared_ptr<AreaLight>& area_light,
									  const MediumInterface& medium_interface)
    : shape(shape),
      material(material),
      area_light(area_light),
      medium_interface(medium_interface)
{}

inline BoundingBoxF GeometricModel::worldSpaceBoundingBox() const
{
    return shape->worldSpaceBoundingBox();
}

inline bool GeometricModel::hasIntersection(const Ray& ray) const
{
    return shape->hasIntersection(ray);
}

inline const AreaLight* GeometricModel::getAreaLight() const
{
    return area_light.get();
}

inline const Material* GeometricModel::getMaterial() const
{
    return material.get();
}

inline void GeometricModel::generateBSDF(SurfaceScatteringEvent* scattering_event,
										 RegionAllocator& allocator,
										 TransportMode transport_mode,
										 bool allow_multiple_scattering_types) const
{
    if (material)
        material->generateBSDF(scattering_event,
                               allocator,
                               transport_mode,
                               allow_multiple_scattering_types);
}

// TransformedModel inline method definitions

inline TransformedModel::TransformedModel(const std::shared_ptr<Model>& model,
										  const AnimatedTransformation& model_to_world)
    : model(model),
      model_to_world(model_to_world)
{}

inline BoundingBoxF TransformedModel::worldSpaceBoundingBox() const
{
    return model_to_world.encompassMotionInBoundingBox(model->worldSpaceBoundingBox());
}

inline const AreaLight* TransformedModel::getAreaLight() const
{
    printSevereMessage("\"getAreaLight()\" method of TransformedModel was called");
    return nullptr;
}

inline const Material* TransformedModel::getMaterial() const
{
    printSevereMessage("\"getMaterial()\" method of TransformedModel was called");
    return nullptr;
}

inline void TransformedModel::generateBSDF(SurfaceScatteringEvent* scattering_event,
										   RegionAllocator& allocator,
										   TransportMode transport_mode,
										   bool allow_multiple_scattering_types) const
{
    printSevereMessage("\"generateBSDF()\" method of TransformedModel was called");
}

// AccelerationStructure inline method definitions

inline const AreaLight* AccelerationStructure::getAreaLight() const
{
    printSevereMessage("\"getAreaLight()\" method of AccelerationStructure was called");
    return nullptr;
}

inline const Material* AccelerationStructure::getMaterial() const
{
    printSevereMessage("\"getMaterial()\" method of AccelerationStructure was called");
    return nullptr;
}

inline void AccelerationStructure::generateBSDF(SurfaceScatteringEvent* scattering_event,
											    RegionAllocator& allocator,
											    TransportMode transport_mode,
											    bool allow_multiple_scattering_types) const
{
    printSevereMessage("\"generateBSDF()\" method of AccelerationStructure was called");
}

} // RayImpact
} // Impact
