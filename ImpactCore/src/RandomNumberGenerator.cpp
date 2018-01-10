#include "random.hpp"

namespace Impact {

// Initialize generator with random seed
std::random_device seed;
extern std::mt19937 IMP_RANDOM_GENERATOR(seed());

// Initialize uniform distribution with range [0, 1)
extern const std::uniform_real_distribution<imp_float> IMP_UNIFORM_DISTRIBUTION(0.0f, 1.0f);

} // Impact
