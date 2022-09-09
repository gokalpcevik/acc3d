#pragma once
#include <Eigen/Eigen>
#include <cstdint>

namespace acc3d::Graphics
{
	using RendererId = uint64_t;
	using LightId = uint64_t;
	using ShaderId = size_t;

	/*
	* This values are guaranteed to never clash with any of the current or future components in the scene.
	*/
	static constexpr RendererId RENDERER_ID_EMPTY_VALUE = 0;
	static constexpr RendererId RENDERER_ID_DELETED_VALUE = 1;

	static constexpr LightId LIGHT_ID_INVALID_VALUE = 0;

	struct Vertex
	{
		Eigen::Vector3f Position{0.0f, 0.0f, 0.0f};
		Eigen::Vector3f Normal{1.0f, 1.0f, 1.0f};
	};

}
