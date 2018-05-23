#include "precision.hpp"

namespace Impact {

// Precision function definitions

int machineIsBigEndian()
{
    union
    {
        uint32_t i;
        char c[4];

    } bint = {0x01020304};

    return bint.c[0] == 1;
}

// Returns the closest float/double that is higher than the given float/double
imp_float closestHigherFloat(imp_float value)
{
    if (std::isinf(value) && value > 0.0)
    {
        // No more values above positive infinity
        return value;
    }

    // Negative zero must be converted to positive zero before continuing
    if (value == -0.0)
        value = 0.0;

    imp_float_bits bits = floatToBits(value);

    // Incrementing or decrementing the bit pattern yields
    // the representation of the closest higher value
    if (value >= 0.0)
        bits++;
    else
        bits--;

    return bitsToFloat(bits);
}

// Returns the closest float/double that is lower than the given float/double
imp_float closestLowerFloat(imp_float value)
{
    if (std::isinf(value) && value < 0.0)
    {
        // No more values below negative infinity
        return value;
    }

    // Positive zero must be converted to negative zero before continuing
    if (value == 0.0)
        value = -0.0;

    imp_float_bits bits = floatToBits(value);

    // Incrementing or decrementing the bit pattern yields
    // the representation of the closest lower value
    if (value <= 0.0)
        bits++;
    else
        bits--;

    return bitsToFloat(bits);
}

} // Impact
