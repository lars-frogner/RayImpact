#pragma once
#include "precision.hpp"
#include "error.hpp"
#include "geometry.hpp"
#include "Transformation.hpp"
#include "ScatteringEvent.hpp"
#include "ParameterSet.hpp"

namespace Impact {
namespace RayImpact {

// Texture declarations

template <typename T>
class Texture {

public:
    virtual T evaluate(const SurfaceScatteringEvent& scattering_event) const = 0;
};

// TextureMapper2D declarations

class TextureMapper2D {

public:
    virtual Point2F textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                                      Vector2F* dstdx, Vector2F* dstdy) const = 0;
};

// ParametricMapper declarations

class ParametricMapper : public TextureMapper2D {

private:

    const imp_float s_scale; // Scaling factor to apply to parametric surface coordinate u for computing texture coordinate s
    const imp_float t_scale; // Scaling factor to apply to parametric surface coordinate v for computing texture coordinate t
    const imp_float s_offset; // Offset to apply when computing texture coordinate s
    const imp_float t_offset; // Offset to apply when computing texture coordinate t

public:

    ParametricMapper(imp_float s_scale, imp_float t_scale,
                     imp_float s_offset, imp_float t_offset);

    Point2F textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                              Vector2F* dstdx, Vector2F* dstdy) const;
};

// SphericalMapper declarations

class SphericalMapper : public TextureMapper2D {

private:

    const Transformation world_to_sphere; // Transformation to apply to points before projecting them onto the unit sphere

    Point2F projectedToSphere(const Point3F& point) const;

public:

    SphericalMapper(const Transformation& world_to_sphere);

    Point2F textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                              Vector2F* dstdx, Vector2F* dstdy) const;
};

// CylindricalMapper declarations

class CylindricalMapper : public TextureMapper2D {

private:

    const Transformation world_to_cylinder; // Transformation to apply to points before projecting them onto the unit cylinder

    Point2F projectedToCylinder(const Point3F& point) const;

public:

    CylindricalMapper(const Transformation& world_to_cylinder);

    Point2F textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                              Vector2F* dstdx, Vector2F* dstdy) const;
};

// PlanarMapper declarations

class PlanarMapper : public TextureMapper2D {

private:

    const Vector3F s_tangent; // Tangent vector specifying the s-axis in the projection plane
    const Vector3F t_tangent; // Tangent vector specifying the t-axis in the projection plane
    const imp_float s_offset; // Offset to apply when computing texture coordinate s
    const imp_float t_offset; // Offset to apply when computing texture coordinate t

public:

    PlanarMapper(const Vector3F& s_tangent, const Vector3F& t_tangent,
                 imp_float s_offset = 0, imp_float t_offset = 0);

    Point2F textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                              Vector2F* dstdx, Vector2F* dstdy) const;
};

// TextureMapper3D declarations

class TextureMapper3D {

public:
    virtual Point3F textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                                      Vector3F* dpdx, Vector3F* dpdy) const = 0;
};

// TransformationMapper declarations

class TransformationMapper {

private:
    const Transformation mapping_transformation; // Transformation to use for mapping world points to 3D texture coordinates

public:

    TransformationMapper(const Transformation& mapping_transformation);

    Point3F textureCoordinate(const SurfaceScatteringEvent& scattering_event,
                              Vector3F* dpdx, Vector3F* dpdy) const;
};

// Texture mapper creation macros

#define create_2D_texture_mapper(parameters, mapper) \
    do { \
        const std::string mapping = parameters.getSingleStringValue("mapping", "parametric"); \
        \
        if (mapping == "parametric") \
        { \
            imp_float s_scale = parameters.getSingleFloatValue("s_scale", 1.0f); \
            imp_float t_scale = parameters.getSingleFloatValue("t_scale", 1.0f); \
            imp_float s_offset = parameters.getSingleFloatValue("s_offset", 0.0f); \
            imp_float t_offset = parameters.getSingleFloatValue("t_offset", 0.0f); \
        \
            mapper.reset(new ParametricMapper(s_scale, t_scale, s_offset, t_offset)); \
        } \
        else if (mapping == "spherical") \
        { \
            mapper.reset(new SphericalMapper(texture_to_world.inverted())); \
        } \
        else if (mapping == "cylindrical") \
        { \
            mapper.reset(new CylindricalMapper(texture_to_world.inverted())); \
        } \
        else if (mapping == "planar") \
        { \
            const Vector3F& s_tangent = parameters.getSingleVector3FValue("s_tangent", Vector3F(1, 0, 0)); \
            const Vector3F& t_tangent = parameters.getSingleVector3FValue("t_tangent", Vector3F(0, 1, 0)); \
            imp_float s_offset = parameters.getSingleFloatValue("s_offset", 0.0f); \
            imp_float t_offset = parameters.getSingleFloatValue("t_offset", 0.0f); \
        \
            mapper.reset(new PlanarMapper(s_tangent, t_tangent, s_offset, t_offset)); \
        } \
        else \
        { \
            printErrorMessage("2D texture mapping \"%s\" is invalid. Using default parametric mapper.", mapping.c_str()); \
            mapper.reset(new ParametricMapper(1, 1, 0, 0)); \
        } \
    } while (false)

} // RayImpact
} // Impact
