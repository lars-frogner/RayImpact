#pragma once
#include "precision.hpp"
#include <atomic>

namespace Impact {

// AtomicFloat declarations

class AtomicFloat {

private:
    std::atomic<imp_float_bits> bits;

public:

    explicit AtomicFloat(imp_float value = 0);

    operator imp_float() const;

    imp_float operator=(imp_float value);

    void add(imp_float value);
};

// AtomicFloat inline method definitions

inline AtomicFloat::AtomicFloat(imp_float value /* = 0 */)
    : bits(floatToBits(value))
{}

inline AtomicFloat::operator imp_float() const
{
    return bitsToFloat(bits);
}

inline imp_float AtomicFloat::operator=(imp_float value)
{
    bits = floatToBits(value);
    return value;
}

} // Impact
