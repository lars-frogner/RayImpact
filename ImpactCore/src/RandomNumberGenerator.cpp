#include "RandomNumberGenerator.hpp"

namespace Impact {

RandomNumberGenerator::RandomNumberGenerator()
	: uniform_continuous_distribution(0.0f, 1.0f),
	  uniform_discrete_distribution()
{
	setRandomSeed();
}

RandomNumberGenerator::RandomNumberGenerator(unsigned int seed)
	: generator(seed),
	  uniform_continuous_distribution(0.0f, 1.0f),
	  uniform_discrete_distribution()
{}

void RandomNumberGenerator::setSeed(unsigned int seed)
{
	generator.seed(seed);
}

void RandomNumberGenerator::setRandomSeed()
{
	std::random_device seed;
	setSeed(seed());
}

imp_float RandomNumberGenerator::uniformFloat()
{
	return uniform_continuous_distribution(generator);
}
	
uint32_t RandomNumberGenerator::uniformUInt32()
{
	return uniform_discrete_distribution(generator);
}
	
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
