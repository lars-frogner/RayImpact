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

} // Impact
