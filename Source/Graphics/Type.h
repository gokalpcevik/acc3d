#pragma once
#include <Eigen/Eigen>
#include <cstdint>
#include <filesystem>

namespace acc3d::Graphics
{
	using RendererId = uint64_t;
	using ShaderId = size_t;
	using RootSignatureId = uint64_t;

	static constexpr RendererId RENDERER_ID_EMPTY_KEY_VALUE = 0;
	static constexpr RendererId RENDERER_ID_DELETED_KEY_VALUE = 1;

	static constexpr ShaderId SHADER_ID_EMPTY_KEY_VALUE = 0;
	static constexpr ShaderId SHADER_ID_DELETED_KEY_VALUE = 1;

	static constexpr RootSignatureId ROOT_SIGNATURE_ID_EMPTY_KEY_VALUE = 0;
	static constexpr RootSignatureId ROOT_SIGNATURE_ID_DELETED_KEY_VALUE = 1;

	// Do not set any entry in this to the values equal to SHADER_ID_EMPTY_KEY_VALUE or SHADER_ID_DELETED_KEY_VALUE
	enum class SHADER_ID_ENTRY_VALUE : ShaderId
	{
		DIFFUSE_VERTEX_SHADER = 0x80D170FD837F8A19ULL,
		DIFFUSE_PIXEL_SHADER = 0xE86AB77D9A936018ULL
	};


	enum class ROOT_SIG_ENTRY_VALUE : RootSignatureId
	{
		DIFFUSE_ROOT_SIGNATURE = 0x33640DDCULL,
		PBR_ROOT_SIGNATURE = 0x1DE76BB22
	};


	struct Vertex
	{
		Eigen::Vector3f Position{0.0f, 0.0f, 0.0f};
		Eigen::Vector3f Normal{1.0f, 1.0f, 1.0f};
	};

	struct RootSignatureInitializer
	{
		std::filesystem::path RootSignaturePath;
		acc3d::Graphics::RootSignatureId RootSignatureId;
	};

}
