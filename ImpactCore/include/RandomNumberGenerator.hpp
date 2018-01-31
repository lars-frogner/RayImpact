#pragma once
#include "precision.hpp"
#include <random>
#include <cstdint>

namespace Impact {

// RandomNumberGenerator declarations

class RandomNumberGenerator {

private:

    std::mt19937 generator; // Mersienne-Twister pseudo-random number generator
    const std::uniform_real_distribution<imp_float> uniform_continuous_distribution;
    const std::uniform_int_distribution<uint32_t> uniform_discrete_distribution;

public:

    RandomNumberGenerator();
    
    RandomNumberGenerator(unsigned int seed);

    void setSeed(unsigned int seed);
    
    void setRandomSeed();

    imp_float uniformFloat();
    
    uint32_t uniformUInt32();
    
    uint32_t uniformUInt32(uint32_t upper_limit);
};

} // Impact
