#pragma once
#include <Eigen/Eigen>
#include <cstdint>

namespace acc3d::Graphics
{
	using RendererId = uint64_t;

	/*
	* This value is guaranteed to never clash with any of the current or future components in the scene.
	*/
	static constexpr RendererId RENDERER_ID_EMPTY_VALUE = 0;
	static constexpr RendererId RENDERER_ID_DELETED_VALUE = 1;

	struct Vertex
	{
		Eigen::Vector3f Position{0.0f, 0.0f, 0.0f};
		Eigen::Vector3f Color{1.0f, 1.0f, 1.0f};
	};

}
