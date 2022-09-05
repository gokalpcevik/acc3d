#pragma once
#include <random>

namespace acc3d::Util
{
	class RandomGenerator
	{
	public:
		RandomGenerator() = default;
		static int64_t RandomInt64();
		static int32_t RandomInt32();
		static uint64_t RandomUInt64();
		static uint32_t RandomUInt32();
		static float RandomFloat();
		static double RandomDouble();

		template<typename T>
		static T RandomIntegral();

		template<typename T>
		static T RandomReal();
	};

	template <typename T>
	T RandomGenerator::RandomIntegral()
	{
		static_assert(std::is_integral_v<T>, "An integral type is required for this function.");
		std::random_device rd;
		std::mt19937_64 mte(rd());

		std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(),
			std::numeric_limits<T>::max());
		return dist(mte);
	}

	template <typename T>
	T RandomGenerator::RandomReal()
	{
		static_assert(std::is_floating_point_v<T>, "A floating point type is required for this function.");
		std::random_device rd;
		std::mt19937_64 mte(rd());

		std::uniform_real_distribution<T> dist(std::numeric_limits<T>::min(),
			std::numeric_limits<T>::max());
		return dist(mte);
	}
}
