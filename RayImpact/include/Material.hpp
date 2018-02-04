#pragma once
#include "precision.hpp"
#include "RegionAllocator.hpp"
#include "geometry.hpp"
#include "ScatteringEvent.hpp"
#include "Texture.hpp"
#include <memory>

namespace Impact {
namespace RayImpact {

// Material declarations

class Material {

public:
    virtual void generateBSDF(SurfaceScatteringEvent* scattering_event,
                              RegionAllocator& allocator,
                              TransportMode transport_mode,
                              bool allow_multiple_scattering_types) const = 0;

    void performBumpMapping(const std::shared_ptr< Texture<imp_float> >& bump_map,
                            SurfaceScatteringEvent* scattering_event) const;
};

} // RayImpact
} // Impact
