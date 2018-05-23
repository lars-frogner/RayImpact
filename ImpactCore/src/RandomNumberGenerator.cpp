#include "RandomNumberGenerator.hpp"

namespace Impact {

// RandomNumberGenerator method definitions

// Returns a random unsigned integer in the range [0, upper_limit-1]
uint32_t RandomNumberGenerator::uniformUInt32(uint32_t upper_limit)
{
    uint32_t threshold = (~upper_limit + 1u) % upper_limit;

    while (true)
    {
        uint32_t number = uniform_discrete_distribution(generator);

        if (number >= threshold)
            return number % upper_limit;
    }
}

} // Impact
