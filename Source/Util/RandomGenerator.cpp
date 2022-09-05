#include "RandomGenerator.h"

namespace acc3d::Util
{
	int64_t RandomGenerator::RandomInt64()
	{
		return RandomIntegral<int64_t>();
	}

	int32_t RandomGenerator::RandomInt32()
	{
		return RandomIntegral<int32_t>();
	}

	uint32_t RandomGenerator::RandomUInt32()
	{
		return RandomIntegral<uint32_t>();
	}

	float RandomGenerator::RandomFloat()
	{
		return RandomReal<float>();
	}

	double RandomGenerator::RandomDouble()
	{
		return RandomReal<double>();
	}

	uint64_t RandomGenerator::RandomUInt64()
	{
		return RandomIntegral<uint64_t>();
	}
}
