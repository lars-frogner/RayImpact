#pragma once
#include "precision.hpp"
#include <random>

namespace Impact {
	
extern std::mt19937 IMP_RANDOM_GENERATOR; // Mersienne-Twister pseudo-random number generator
	
// Note: The functions using IMP_RANDOM_GENERATOR are not thread-safe,
// since the generator object is shared among all threads.

extern const std::uniform_real_distribution<imp_float> IMP_UNIFORM_DISTRIBUTION;

inline void set_random_seed(unsigned int new_seed)
{
	IMP_RANDOM_GENERATOR.seed(new_seed);
}

inline imp_float random()
{
	return IMP_UNIFORM_DISTRIBUTION(IMP_RANDOM_GENERATOR);
}

inline imp_float random_range(imp_float start, imp_float end)
{
	return start + random()*(end - start);
}

} // Impact