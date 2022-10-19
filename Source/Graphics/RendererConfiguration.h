#pragma once

#include <cstdint>

namespace acc3d::Graphics
{
    static constexpr uint64_t g_NUM_FRAMES_IN_FLIGHT = 3ULL;
	static constexpr size_t g_MAX_NUM_OF_DIR_LIGHTS = 32ULL;

	static constexpr size_t g_LIGHT_CONTEXT_STATIC_DESCRIPTOR_HEAP_SLOT = 2ULL;
}
