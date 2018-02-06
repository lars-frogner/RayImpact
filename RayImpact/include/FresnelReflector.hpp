#pragma once
#include "precision.hpp"
#include "geometry.hpp"
#include "Spectrum.hpp"

namespace Impact {
namespace RayImpact {

// FresnelReflector utility functions

imp_float fresnelReflectance(imp_float cos_incident_angle,
                             imp_float refractive_index_outside,
                             imp_float refractive_index_inside);

ReflectionSpectrum fresnelReflectance(imp_float cos_incident_angle,
                                      const Spectrum& refractive_index_outside,
                                      const Spectrum& refractive_index_inside,
                                      const Spectrum& absorption_coefficient_inside);

// FresnelReflector declarations

class FresnelReflector {

public:
    virtual ReflectionSpectrum evaluate(imp_float cos_incident_angle) const = 0;
};

// PerfectReflector implementation

class PerfectReflector : public FresnelReflector {

public:

    ReflectionSpectrum evaluate(imp_float cos_incident_angle) const
    {
        return ReflectionSpectrum(1.0f);
    }
};

// DielectricReflector implementation

class DielectricReflector : public FresnelReflector {

private:
    imp_float refractive_index_outside; // Index of refraction of the medium on the outside of the object
    imp_float refractive_index_inside; // Index of refraction of the medium on the inside of the object

public:

    DielectricReflector(imp_float refractive_index_outside,
                        imp_float refractive_index_inside)
        : refractive_index_outside(refractive_index_outside),
          refractive_index_inside(refractive_index_inside)
    {}

    ReflectionSpectrum evaluate(imp_float cos_incident_angle) const
    {
        return ReflectionSpectrum(fresnelReflectance(cos_incident_angle,
                                                     refractive_index_outside,
                                                     refractive_index_inside));
    }
};

// ConductiveReflector implementation

class ConductiveReflector : public FresnelReflector {

private:
    Spectrum refractive_index_outside; // Index of refraction of the medium on the outside of the object
    Spectrum refractive_index_inside; // Index of refraction of the medium on the inside of the object
    Spectrum absorption_coefficient_inside; // Absorption coefficient of the conductive medium on the inside of the object

public:

    ConductiveReflector(const Spectrum& refractive_index_outside,
                        const Spectrum& refractive_index_inside,
                        const Spectrum& absorption_coefficient_inside)
        : refractive_index_outside(refractive_index_outside),
          refractive_index_inside(refractive_index_inside),
          absorption_coefficient_inside(absorption_coefficient_inside)
    {}

    ReflectionSpectrum operator()(imp_float cos_incident_angle) const
    {
        return fresnelReflectance(std::abs(cos_incident_angle),
                                  refractive_index_outside,
                                  refractive_index_inside,
                                  absorption_coefficient_inside);
    }
};

} // RayImpact
} // Impact
