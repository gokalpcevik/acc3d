#pragma once
#include <Eigen/Eigen>
#include <cstdint>

namespace acc3d::Graphics
{
	using RendererId = uint64_t;
	using ShaderId = size_t;

	static constexpr RendererId RENDERER_ID_EMPTY_KEY_VALUE = 0;
	static constexpr RendererId RENDERER_ID_DELETED_KEY_VALUE = 1;

	static constexpr ShaderId SHADER_ID_EMPTY_KEY_VALUE = 0;
	static constexpr ShaderId SHADER_ID_DELETED_KEY_VALUE = 1;

	// Do not set any entry in this to the values equal to SHADER_ID_EMPTY_KEY_VALUE or SHADER_ID_DELETED_KEY_VALUE
	enum SHADER_ID_ENTRY_VALUE : ShaderId
	{
		ACC3D_DIFFUSE_VERTEX_SHADER = 0x80D170FD837F8A19,
		ACC3D_DIFFUSE_PIXEL_SHADER = 0xE86AB77D9A936018
	};


	struct Vertex
	{
		Eigen::Vector3f Position{0.0f, 0.0f, 0.0f};
		Eigen::Vector3f Normal{1.0f, 1.0f, 1.0f};
	};

}
