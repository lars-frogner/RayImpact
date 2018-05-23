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

// RandomNumberGenerator inline method definitions

inline RandomNumberGenerator::RandomNumberGenerator()
    : uniform_continuous_distribution(0.0f, 1.0f),
      uniform_discrete_distribution()
{
    setRandomSeed();
}

inline RandomNumberGenerator::RandomNumberGenerator(unsigned int seed)
    : generator(seed),
      uniform_continuous_distribution(0.0f, 1.0f),
      uniform_discrete_distribution()
{}

inline void RandomNumberGenerator::setSeed(unsigned int seed)
{
    generator.seed(seed);
}

inline void RandomNumberGenerator::setRandomSeed()
{
    std::random_device seed;
    setSeed(seed());
}

inline imp_float RandomNumberGenerator::uniformFloat()
{
    return uniform_continuous_distribution(generator);
}

inline uint32_t RandomNumberGenerator::uniformUInt32()
{
    return uniform_discrete_distribution(generator);
}

} // Impact
